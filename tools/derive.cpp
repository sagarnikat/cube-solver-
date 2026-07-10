// Ground-truth 3D cube simulator used to DERIVE and VERIFY the fast cubie model.
//
// Everything here is defined from unambiguous 3D geometry (positions + rotation
// matrices). We use it to:
//   1. print the sticker-ordering tables (CornerFaces / EdgeFaces),
//   2. print each move as a permutation + orientation-delta,
//   3. reimplement the fast piece-model from those tables and check it matches
//      the 3D reference over thousands of random sequences.
//
// Build: g++ -std=c++20 tools/derive.cpp -o /tmp/derive && /tmp/derive
//
// Colour/axis convention (matches notes.txt):
//   U=white(0) +y   D=yellow(5) -y   L=green(1) -x   R=blue(3) +x
//   F=red(2)   +z   B=orange(4) -z
#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <cstdint>
#include <cassert>
using namespace std;

using V3  = array<int,3>;
using Mat = array<array<int,3>,3>;   // rows

static V3 matvec(const Mat& m, const V3& v){
    V3 r{};
    for(int i=0;i<3;i++) r[i]=m[i][0]*v[0]+m[i][1]*v[1]+m[i][2]*v[2];
    return r;
}
static Mat matmul(const Mat& a, const Mat& b){
    Mat r{};
    for(int i=0;i<3;i++)for(int j=0;j<3;j++){int s=0;for(int k=0;k<3;k++)s+=a[i][k]*b[k][j];r[i][j]=s;}
    return r;
}
static const Mat I3{{{1,0,0},{0,1,0},{0,0,1}}};

// Clockwise-from-outside rotation matrices for each face (see reasoning in notes).
// R:(x,y,z)->(x,z,-y)  U:->(-z,y,x)  F:->(y,-x,z)  L:->(x,-z,y)  D:->(z,y,-x)  B:->(-y,x,z)
static const Mat MR{{{1,0,0},{0,0,1},{0,-1,0}}};
static const Mat MU{{{0,0,-1},{0,1,0},{1,0,0}}};
static const Mat MF{{{0,1,0},{-1,0,0},{0,0,1}}};
static const Mat ML{{{1,0,0},{0,0,-1},{0,1,0}}};
static const Mat MD{{{0,0,1},{0,1,0},{-1,0,0}}};
static const Mat MB{{{0,-1,0},{1,0,0},{0,0,1}}};

struct Face { char name; Mat rot; int axis; int sign; };
static const array<Face,6> FACES = {{
    {'U',MU,1,+1}, {'D',MD,1,-1},
    {'L',ML,0,-1}, {'R',MR,0,+1},
    {'F',MF,2,+1}, {'B',MB,2,-1},
}};

static int normalToFace(const V3& n){
    if(n[1]==+1) return 0; // U white
    if(n[1]==-1) return 5; // D yellow
    if(n[0]==-1) return 1; // L green
    if(n[0]==+1) return 3; // R blue
    if(n[2]==+1) return 2; // F red
    if(n[2]==-1) return 4; // B orange
    assert(false); return -1;
}
static int axisOf(const V3& n){ for(int i=0;i<3;i++) if(n[i]!=0) return i; return -1; }

// Project cubicle coordinates ---------------------------------------------------
// Corners: c0..c7
static const array<V3,8> CORNER_POS = {{
    {-1,+1,-1},{+1,+1,-1},{-1,+1,+1},{+1,+1,+1},   // ULB URB ULF URF
    {-1,-1,-1},{+1,-1,-1},{-1,-1,+1},{+1,-1,+1},   // DLB DRB DLF DRF
}};
// Edges: e0..e11
static const array<V3,12> EDGE_POS = {{
    {0,+1,-1},{-1,+1,0},{+1,+1,0},{0,+1,+1},        // UB UL UR UF
    {-1,0,-1},{+1,0,-1},{-1,0,+1},{+1,0,+1},        // LB RB LF RF
    {0,-1,-1},{-1,-1,0},{+1,-1,0},{0,-1,+1},        // DB DL DR DF
}};

// Determinant of 3 column vectors
static int det3(const V3&a,const V3&b,const V3&c){
    return a[0]*(b[1]*c[2]-b[2]*c[1])
         - a[1]*(b[0]*c[2]-b[2]*c[0])
         + a[2]*(b[0]*c[1]-b[1]*c[0]);
}

// Clockwise-consistent sticker face ordering for a corner cubicle.
// slot0 = U/D face; the two side faces ordered so (u,a,b) is right-handed.
static array<int,3> cornerFaceOrder(const V3& P){
    V3 u{0,P[1],0};
    V3 nx{P[0],0,0}, nz{0,0,P[2]};
    array<int,3> out;
    out[0]=normalToFace(u);
    if(det3(u,nx,nz)>0){ out[1]=normalToFace(nx); out[2]=normalToFace(nz); }
    else               { out[1]=normalToFace(nz); out[2]=normalToFace(nx); }
    return out;
}
// slot ordering as VECTORS (for orientation extraction)
static array<V3,3> cornerSlotVecs(const V3& P){
    V3 u{0,P[1],0}, nx{P[0],0,0}, nz{0,0,P[2]};
    if(det3(u,nx,nz)>0) return {u,nx,nz};
    return {u,nz,nx};
}
// Edge cubicle sticker faces: slot0 = U/D if present else L/R; slot1 = the other.
static array<int,2> edgeFaceOrder(const V3& P){
    if(P[1]!=0){
        V3 side = P[0]!=0 ? V3{P[0],0,0} : V3{0,0,P[2]};
        return { normalToFace({0,P[1],0}), normalToFace(side) };
    }
    return { normalToFace({P[0],0,0}), normalToFace({0,0,P[2]}) };
}
static V3 edgeSlot0Vec(const V3& P){ if(P[1]!=0) return {0,P[1],0}; return {P[0],0,0}; }

// Reference cube ---------------------------------------------------------------
struct Cubie { int home; V3 pos; Mat ori; };
struct Ref {
    array<Cubie,8>  C;
    array<Cubie,12> E;
    Ref(){
        for(int i=0;i<8;i++)  C[i]={i,CORNER_POS[i],I3};
        for(int i=0;i<12;i++) E[i]={i,EDGE_POS[i],I3};
    }
    void turn(char f){
        const Face* F=nullptr; for(auto&x:FACES) if(x.name==f) F=&x;
        for(auto&c:C) if(c.pos[F->axis]==F->sign){ c.pos=matvec(F->rot,c.pos); c.ori=matmul(F->rot,c.ori); }
        for(auto&e:E) if(e.pos[F->axis]==F->sign){ e.pos=matvec(F->rot,e.pos); e.ori=matmul(F->rot,e.ori); }
    }
    // Extract project-model state (pieceID + orientation per cubicle).
    void extract(array<int,8>&cp,array<int,8>&co,array<int,12>&ep,array<int,12>&eo) const {
        for(int slot=0;slot<8;slot++){
            const V3& P=CORNER_POS[slot];
            const Cubie* c=nullptr; for(auto&x:C) if(x.pos==P) c=&x;
            cp[slot]=c->home;
            // U/D-colour sticker of this cubie: home normal (0, homeY, 0)
            V3 homeUD{0,CORNER_POS[c->home][1],0};
            V3 cur=matvec(c->ori,homeUD);
            auto vs=cornerSlotVecs(P);
            int o=-1; for(int s=0;s<3;s++) if(vs[s]==cur) o=s;
            co[slot]=o;
        }
        for(int slot=0;slot<12;slot++){
            const V3& P=EDGE_POS[slot];
            const Cubie* e=nullptr; for(auto&x:E) if(x.pos==P) e=&x;
            ep[slot]=e->home;
            // ref sticker of this cubie: U/D sticker if it has one else L/R sticker
            V3 hp=EDGE_POS[e->home];
            V3 homeRef = hp[1]!=0 ? V3{0,hp[1],0} : V3{hp[0],0,0};
            V3 cur=matvec(e->ori,homeRef);
            eo[slot]= (axisOf(cur)==axisOf(edgeSlot0Vec(P))) ? 0 : 1;
        }
    }
    bool solved() const {
        for(auto&c:C) if(!(c.pos==CORNER_POS[c.home] && c.ori==I3)) return false;
        for(auto&e:E) if(!(e.pos==EDGE_POS[e.home] && e.ori==I3)) return false;
        return true;
    }
};

// ---- Fast piece model, rebuilt from derived per-move tables ------------------
struct MoveData {
    array<int,8>  cp; array<int,8>  co;   // dest<-src permutation & orientation delta
    array<int,12> ep; array<int,12> eo;
};
static array<MoveData,6> deriveMoves(){
    const char faces[6]={'U','D','L','R','F','B'};
    array<MoveData,6> md;
    for(int m=0;m<6;m++){ Ref r; r.turn(faces[m]); r.extract(md[m].cp,md[m].co,md[m].ep,md[m].eo); }
    return md;
}
static int MFACE(char f){ const char s[6]={'U','D','L','R','F','B'}; for(int i=0;i<6;i++) if(s[i]==f) return i; return -1; }

struct Piece { array<uint8_t,8> cp,co; array<uint8_t,12> ep,eo; };
static Piece solvedPiece(){ Piece p; for(int i=0;i<8;i++){p.cp[i]=i;p.co[i]=0;} for(int i=0;i<12;i++){p.ep[i]=i;p.eo[i]=0;} return p; }
static void applyPiece(Piece&p,const MoveData&m){
    Piece n=p;
    for(int d=0;d<8;d++){ int s=m.cp[d]; n.cp[d]=p.cp[s]; n.co[d]=(p.co[s]+m.co[d])%3; }
    for(int d=0;d<12;d++){ int s=m.ep[d]; n.ep[d]=p.ep[s]; n.eo[d]=p.eo[s]^m.eo[d]; }
    p=n;
}

int main(){
    // Reference validity: every face order 4, sexy move order 6.
    auto order=[&](const string&alg){
        Ref r; int k=0;
        do{
            for(size_t i=0;i<alg.size();){
                char f=alg[i++]; int t=1;
                if(i<alg.size()&&alg[i]=='2'){t=2;i++;}
                else if(i<alg.size()&&alg[i]=='\''){t=3;i++;}
                for(int q=0;q<t;q++) r.turn(f);
            }
            k++;
        }while(!r.solved()&&k<10000);
        return k;
    };
    cout<<"Reference order checks (expect 4 and 6):\n";
    for(char f:string("UDLRFB")) cout<<"  "<<f<<"^n = "<<order(string(1,f))<<"\n";
    cout<<"  (RUR'U')^n = "<<order("RUR'U'")<<"\n";
    cout<<"  (FUF'U')^n = "<<order("FUF'U'")<<"\n";
    cout<<"  (LDL'D')^n = "<<order("LDL'D'")<<"\n";
    cout<<"  (BRB'R')^n = "<<order("BRB'R'")<<"\n";
    cout<<"  (R'D'RD)^n = "<<order("R'D'RD")<<"\n";

    // Print tables
    cout<<"\n// clockwise-consistent CornerFaces[8][3]  {U/D, side, side}\nconst int CornerFaces[8][3] = {\n";
    for(int i=0;i<8;i++){ auto o=cornerFaceOrder(CORNER_POS[i]); cout<<"    {"<<o[0]<<","<<o[1]<<","<<o[2]<<"},\n"; }
    cout<<"};\n";
    cout<<"const int EdgeFaces[12][2] = {\n";
    for(int i=0;i<12;i++){ auto o=edgeFaceOrder(EDGE_POS[i]); cout<<"    {"<<o[0]<<","<<o[1]<<"},\n"; }
    cout<<"};\n";

    // Print moves
    auto md=deriveMoves();
    const char faces[6]={'U','D','L','R','F','B'};
    cout<<"\n// ---- derived moves (dest<-src perm, +delta) ----\n";
    for(int m=0;m<6;m++){
        cout<<"move_"<<faces[m]<<":\n  corner dest<-src / +o : ";
        for(int d=0;d<8;d++) if(md[m].cp[d]!=d||md[m].co[d]) cout<<d<<"<-"<<md[m].cp[d]<<"(+"<<md[m].co[d]<<") ";
        cout<<"\n  edge   dest<-src / ^o : ";
        for(int d=0;d<12;d++) if(md[m].ep[d]!=d||md[m].eo[d]) cout<<d<<"<-"<<md[m].ep[d]<<"(^"<<md[m].eo[d]<<") ";
        cout<<"\n";
    }

    // Equivalence test: piece model vs reference over random sequences.
    cout<<"\nEquivalence test (piece model vs 3D reference):\n";
    uint64_t rng=0x12345678;
    auto rnd=[&](){ rng^=rng<<13; rng^=rng>>7; rng^=rng<<17; return rng; };
    int fails=0;
    for(int t=0;t<200000;t++){
        Ref r; Piece p=solvedPiece();
        int len=1+rnd()%25;
        for(int s=0;s<len;s++){
            int f=rnd()%6; int turns=1+rnd()%3;
            for(int q=0;q<turns;q++){ r.turn(faces[f]); applyPiece(p,md[f]); }
        }
        array<int,8>cp,co; array<int,12>ep,eo; r.extract(cp,co,ep,eo);
        bool ok=true;
        for(int i=0;i<8;i++)  if(cp[i]!=p.cp[i]||co[i]!=p.co[i]) ok=false;
        for(int i=0;i<12;i++) if(ep[i]!=p.ep[i]||eo[i]!=p.eo[i]) ok=false;
        if(!ok){ fails++; if(fails<=3) cout<<"  MISMATCH at test "<<t<<"\n"; }
    }
    cout<<(fails? "  FAILED: "+to_string(fails)+" mismatches\n" : "  ALL 200000 RANDOM SEQUENCES MATCH ✓\n");

    // Display-formula check: render each corner/edge sticker from the piece model
    // using color = CornerFaces[piece][(s-o+3)%3] and compare to the actual
    // home-colour of the sticker physically sitting at that cubicle slot.
    cout<<"\nDisplay-formula check (render vs actual sticker colour):\n";
    array<array<int,3>,8> CF; for(int i=0;i<8;i++) CF[i]=cornerFaceOrder(CORNER_POS[i]);
    array<array<int,2>,12> EF; for(int i=0;i<12;i++) EF[i]=edgeFaceOrder(EDGE_POS[i]);
    int dfails=0;
    for(int t=0;t<200000;t++){
        Ref r; Piece p=solvedPiece();
        int len=1+rnd()%25;
        for(int s=0;s<len;s++){ int f=rnd()%6,tr=1+rnd()%3; for(int q=0;q<tr;q++){ r.turn(faces[f]); applyPiece(p,md[f]); } }
        // actual colours at each corner cubicle slot from the reference
        for(int slot=0;slot<8;slot++){
            const V3&P=CORNER_POS[slot];
            const Cubie* c=nullptr; for(auto&x:r.C) if(x.pos==P) c=&x;
            auto vs=cornerSlotVecs(P);
            for(int sl=0;sl<3;sl++){
                // which sticker of cubie c points along vs[sl]? its home normal n with ori*n==vs[sl]
                int actual=-1;
                for(int ax=0;ax<3;ax++){ V3 hn{0,0,0}; hn[ax]=CORNER_POS[c->home][ax]; if(hn[ax]==0) continue; if(matvec(c->ori,hn)==vs[sl]) actual=normalToFace(hn); }
                int rendered=CF[p.cp[slot]][(sl - p.co[slot] + 3)%3];
                if(actual!=rendered) dfails++;
            }
        }
        for(int slot=0;slot<12;slot++){
            const V3&P=EDGE_POS[slot];
            const Cubie* e=nullptr; for(auto&x:r.E) if(x.pos==P) e=&x;
            V3 s0=edgeSlot0Vec(P); V3 s1{}; // slot1 = the other axis of P
            for(int ax=0;ax<3;ax++){ if(P[ax]!=0 && !(V3{ (ax==0?P[0]:0),(ax==1?P[1]:0),(ax==2?P[2]:0)}==s0)) { s1={0,0,0}; s1[ax]=P[ax]; } }
            V3 slots[2]={s0,s1};
            for(int sl=0;sl<2;sl++){
                int actual=-1;
                for(int ax=0;ax<3;ax++){ V3 hn{0,0,0}; hn[ax]=EDGE_POS[e->home][ax]; if(hn[ax]==0) continue; if(matvec(e->ori,hn)==slots[sl]) actual=normalToFace(hn); }
                int rendered=EF[p.ep[slot]][(sl - p.eo[slot] + 2)%2];
                if(actual!=rendered) dfails++;
            }
        }
    }
    cout<<(dfails? "  FAILED: "+to_string(dfails)+" sticker mismatches\n" : "  ALL rendered stickers MATCH reference ✓\n");

    // Emit a data-driven facelet grid[6 faces][3 rows][3 cols] = {kind,idx,slot}
    // kind: 0=center 1=corner 2=edge. Net matches the current U / L F R B / D layout.
    cout<<"\n// ---- Facelet grid for display (paste into display.cpp) ----\n";
    struct FaceDef{ char name; V3 N; V3 o; V3 dr; V3 dc; };
    // P(r,c) = o + r*dr + c*dc ; N = outward normal
    array<FaceDef,6> fd = {{
        {'U',{0,1,0},  {-1,1,-1},{0,0,1},{1,0,0}},   // rows back->front, cols left->right
        {'L',{-1,0,0}, {-1,1,-1},{0,-1,0},{0,0,1}},  // rows up->down,   cols back->front
        {'F',{0,0,1},  {-1,1,1}, {0,-1,0},{1,0,0}},  // rows up->down,   cols left->right
        {'R',{1,0,0},  {1,1,1},  {0,-1,0},{0,0,-1}}, // rows up->down,   cols front->back
        {'B',{0,0,-1}, {1,1,-1}, {0,-1,0},{-1,0,0}}, // rows up->down,   cols right->left
        {'D',{0,-1,0}, {-1,-1,1},{0,0,-1},{1,0,0}},  // rows front->back,cols left->right
    }};
    auto findCorner=[&](const V3&P){ for(int i=0;i<8;i++) if(CORNER_POS[i]==P) return i; return -1; };
    auto findEdge  =[&](const V3&P){ for(int i=0;i<12;i++) if(EDGE_POS[i]==P) return i; return -1; };
    cout<<"//              kind(0=ctr,1=corner,2=edge), index, slot\n";
    for(auto&f:fd){
        cout<<"    { // "<<f.name<<"\n";
        for(int r=0;r<3;r++){
            cout<<"      ";
            for(int c=0;c<3;c++){
                V3 P{ f.o[0]+r*f.dr[0]+c*f.dc[0], f.o[1]+r*f.dr[1]+c*f.dc[1], f.o[2]+r*f.dr[2]+c*f.dc[2] };
                int nz=(P[0]!=0)+(P[1]!=0)+(P[2]!=0);
                int nf=normalToFace(f.N);
                if(nz==1){ cout<<"{0,"<<nf<<",0}, "; }
                else if(nz==3){ int ci=findCorner(P); auto ord=cornerFaceOrder(P); int sl=0; for(int s=0;s<3;s++) if(ord[s]==nf) sl=s; cout<<"{1,"<<ci<<","<<sl<<"}, "; }
                else { int ei=findEdge(P); auto ord=edgeFaceOrder(P); int sl=0; for(int s=0;s<2;s++) if(ord[s]==nf) sl=s; cout<<"{2,"<<ei<<","<<sl<<"}, "; }
            }
            cout<<"\n";
        }
        cout<<"    },\n";
    }
    return 0;
}
