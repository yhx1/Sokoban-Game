#include<iostream>
#include<vector>
#include<string>
#include<unordered_set>
#include<unordered_map>
#include<sstream>
#include<utility>
#include<queue>
#include<fstream>
#include<stack>
#include<climits>
#include<algorithm>

using namespace std;

#include "test.h"


struct pair_hash {
    inline std::size_t operator()(const std::pair<int,int> & v) const {
        return v.first*31+v.second;
    }
};

class Sokoban{

    private:

        int sizeH;
        int sizeW;
        int walls; //number of walls
        int boxes; //number of boxes
        int storages; //number of storages

        string initState;
        unordered_set<string> finalStates;
        unordered_set<pair<int,int>, pair_hash> storageLocs;

        vector<vector<int>> boundry; //0:unreachable, -1:uncertainty, 1 reachable

        vector<pair<int,int>> dirt{{-1,0}, {1,0}, {0,-1}, {0,1} }; //four directions

        void initSize(vector<vector<char>> &board, string boardSize){

            stringstream ss;
            ss<<boardSize;

            ss>>sizeH;
            ss>>sizeW;

            board.resize(sizeH, vector<char>(sizeW,' '));
        }

        void setWalls(vector<vector<char>> &board){

            for(int i=0;i<sizeW;i++){
                board[0][i] = '#';
                board[sizeH-1][i] = '#';
            }

            for(int i=0;i<sizeH;i++){
                board[i][0] = '#';
                board[i][sizeW-1] = '#';
            }

        }

        void initWalls(vector<vector<char>> &board, string nWallSquares){

            stringstream ss;
            ss<<nWallSquares;

            ss>>walls;

            for (int i=0; i<walls; i++){
                int x,y;
                ss>>x;
                x-=1;
                ss>>y;
                y-=1;
                board[y][x] = '#';
            }

            //set walls to bound the box if not given by input
            //setWalls(board);

        }

        void initBoxes(vector<vector<char>> &board, string nBoxes){

            stringstream ss;
            ss<<nBoxes;

            ss>>boxes;

            for (int i=0; i<boxes; i++){
                int x,y;
                ss>>x;
                x-=1;
                ss>>y;
                y-=1;
                board[y][x] = '$';
            }
        }

        void initStorageLoc(vector<vector<char>>&board, string nStorageLocations){

            stringstream ss;
            ss<<nStorageLocations;

            ss>>storages;

            for (int i=0; i<storages; i++){
                int x,y;
                ss>>x;
                x-=1;
                ss>>y;
                y-=1;
                board[y][x] = '.';
                storageLocs.insert({y,x});
            }
        }

        void initPlayerLoc(vector<vector<char>> &board, string _playerLoc){

            stringstream ss;
            ss<<_playerLoc;

            int x,y;
            ss>>x;
            x-=1;
            ss>>y;
            y-=1;

            board[y][x] = '@';

        }

        bool isBoundry(vector<vector<char>> &board, int i, int j, pair<int,int> d){

            if(!inbox(i,j) || board[i][j]=='#' || board[i][j]=='.' || boundry[i][j]==1)
                return false;

            if(boundry[i][j]==0)
                return true;

            int ty=i+d.first;
            int tx=j+d.second;

            if(isBoundry(board, ty, tx, d)){
                boundry[i][j] = 0;
                return true;
            }else{
                boundry[i][j] = 1;
                return false;
            }
        }

        void initBoundry(vector<vector<char>> &board){

            boundry.resize(sizeH, vector<int>(sizeW,1));

            //change corners to 0(unreachable)
            //and mark spaces with one wall neighbor as -1(uncertainty)
            for(int i=0; i<sizeH; i++){
                for(int j=0; j<sizeW; j++){

                    if(board[i][j]=='#'){
                        boundry[i][j] = 0;

                    }else if(board[i][j]==' ' || board[i][j]=='@'){

                        int cnt_x=0;
                        int cnt_y=0;

                        for(int k=0;k<4;k++){

                            int ty=i+dirt[k].first;
                            int tx=j+dirt[k].second;

                            if(inbox(ty, tx) && board[ty][tx]=='#'){
                                if(k==0 || k==1) cnt_y++;
                                else cnt_x++;
                            }
                        }

                        if(cnt_y>=1 && cnt_x>=1){
                            boundry[i][j] = 0;
                        }else if(cnt_y>=1 || cnt_x>=1){
                            boundry[i][j] = -1;
                        }
                    }
                }
            }

            //change uncertainty to either reachable or unreachable
            for(int i=0;i<sizeH;i++){
                for(int j=0;j<sizeW;j++){
                    if(boundry[i][j]==0 && board[i][j]!='#'){
                        //walk straight line in 4 directions
                        for(int k=0;k<4;k++){

                            pair<int,int> d=dirt[k];
                            int ty=i+d.first;
                            int tx=j+d.second;

                            isBoundry(board, ty, tx, d);
                        }
                    }else if(boundry[i][j]==-1){
                        boundry[i][j]=1;
                    }
                }
            }
        }

        void initFrom2DArray(vector<vector<char>> &board){

            sizeH=board.size();
            sizeW=board[0].size();

            for(int i=0;i<sizeH;i++){
                for(int j=0;j<sizeW;j++){
                    if(board[i][j]=='#')
                        walls++;
                    else if(board[i][j]=='$')
                        boxes++;
                    else if(board[i][j]=='.'){
                        storageLocs.insert({i,j});
                        storages++;
                    }
                }
            }

        }

        bool inbox(int y, int x){
            return y>=0 && x>=0 && y<sizeH && x<sizeW;
        }

        void addPlayerLoc(vector<vector<char>> board, int i, int j){

            for(int k=0;k<4;k++){

                int ty=i+dirt[k].first;
                int tx=j+dirt[k].second;

                if(inbox(ty, tx) && board[ty][tx]==' '){
                    board[ty][tx] = '@';
                    string tmp = encode(board);
                    finalStates.insert(tmp);
                    board[ty][tx] = ' ';
                }
            }
        }

        void initFinals(vector<vector<char>> board){

            //move boxes to goals and clear player's position on the board
            for(int i=0;i<sizeH;i++){
                for(int j=0;j<sizeW;j++){

                    if(board[i][j]=='.')
                        board[i][j] = '$';
                    else if(board[i][j]=='$')
                        board[i][j] = ' ';
                    else if(board[i][j]=='@')
                        board[i][j] = ' ';
                }
            }

            for(int i=0;i<sizeH;i++){
                for(int j=0;j<sizeW;j++){

                    if(board[i][j]=='$'){
                        addPlayerLoc(board, i, j);
                    }
                }
            }
        }

        //encode two dimensional vector into string
        string encode(vector<vector<char>> &board){

            string res;

            for(auto b:board){
                for(char c:b){
                    res.push_back(c);
                }
            }

            return res;

        }

        //decode string to two dimensional vector
        vector<vector<char>> decode(string &s){

            vector<vector<char>> res(sizeH, vector<char>(sizeW, ' '));

            for(int i=0;i<sizeH;i++){
                for(int j=0;j<sizeW;j++){
                    res[i][j]=s[i*sizeW+j];
                }
            }

            return res;

        }

        pair<int,int> getPlayerLoc(vector<vector<char>> &board){

            for(int i=0;i<board.size();i++){
                for(int j=0;j<board[0].size();j++){

                    if(board[i][j]=='@'){
                        return {i,j};
                    }
                }
            }

            return {-1,-1};
        }

        bool validPos(int by, int bx){
            return boundry[by][bx];
        }

        bool moveBox(vector<vector<char>> &board, int by, int bx){

            if(!inbox(by, bx) || board[by][bx]=='#' || board[by][bx]=='$' || !validPos(by, bx))
                return false;

            return true;
        }

        int restrictedDirections(vector<vector<char>> &board, int by, int bx) {
          bool resY = false;
          bool resX = false;
          for(int k=0;k<4;k++){
            int ty = by+dirt[k].first;
            int tx = bx+dirt[k].second;
            if (board[ty][tx] == '#' || board[ty][tx] == '$'){
              if (k<2) resY = true;
              else resX = true;
            }
          }
          if (resY&&resX) return 3;
          if (resY) return 1;
          if (resX) return 2;
          return 0;
        }

        bool detectAdjacentBoxDeadlock(vector<vector<char>> &board, int by, int bx) {
          int curRes = restrictedDirections(board, by, bx);
          if (curRes == 0) return false;
          for(int k=0;k<4;k++){
            int ty = by+dirt[k].first;
            int tx = bx+dirt[k].second;
            if (board[ty][tx] == '$'){
              int nextRes = restrictedDirections(board, ty, tx);
              if ( (curRes == 3) && (nextRes == 3)
                //( (curRes == 1) && (nextRes == 1) && (k>1) ) || ((curRes == 1) && (nextRes == 3) && (k>1)) || ((curRes == 2) && (nextRes == 2) && (k<2)) || ((curRes == 2) && (nextRes == 3) && (k<2))
              ){
                return true;
              }
            }
          }
          return false;
        }

        void getNextStatesBFS(string cur, queue<string>& q, unordered_map<string,string> &parent,  unordered_set<string> &visited){

            visited.insert(cur);
            vector<vector<char>> board = decode(cur);

            pair<int,int> playerLoc = getPlayerLoc(board);
            int y=playerLoc.first;
            int x=playerLoc.second;

            if(storageLocs.count({y,x}))
                board[y][x] = '.';
            else
                board[y][x] = ' ';

            for(int k=0;k<4;k++){

                int ty = y+dirt[k].first;
                int tx = x+dirt[k].second;

                if(inbox(ty, tx) && board[ty][tx]!='#'){

                    if(board[ty][tx]=='$'){
                        int by=ty+dirt[k].first;
                        int bx=tx+dirt[k].second;

                        if(moveBox(board, by, bx)){

                            char c1=board[ty][tx];
                            board[ty][tx]='@';

                            char c2=board[by][bx];
                            board[by][bx]='$';

                            if (c2!= '.' && detectAdjacentBoxDeadlock(board, by, bx)) {
                              board[ty][tx]=c1;
                              board[by][bx]=c2;
                              continue;
                            }

                            string tmp=encode(board);
                            board[ty][tx]=c1;
                            board[by][bx]=c2;

                            if(visited.count(tmp)) continue;

                            parent[tmp]=cur;
                            q.push(tmp);
                        }

                    }else{

                        char c=board[ty][tx];
                        board[ty][tx]='@';
                        string tmp=encode(board);
                        board[ty][tx]=c;

                        if(visited.count(tmp)) continue;

                        parent[tmp]=cur;
                        q.push(tmp);
                    }
                }
            }
        }

        void BFS(){

            queue<string> q;
            q.push(initState);

            unordered_set<string> visited;
            int steps=0;

            unordered_map<string, string> parent;
            parent[initState] = initState;

            while(!q.empty()){

                int len=q.size();
                steps++;

                for(int i=0;i<len;i++){

                    string cur=q.front();
                    q.pop();

                    //find the solution
                    if(finalStates.count(cur)){
                        print(parent, cur);
                        return ;
                    }

                    getNextStatesBFS(cur, q, parent, visited);

                }
            }

            cout<<"No answer!"<<endl;
        }

        void getNextStatesDFS(string cur, stack<string>& stk, unordered_map<string,string> &parent,  unordered_set<string> &visited){

            visited.insert(cur);
            vector<vector<char>> board = decode(cur);

            pair<int,int> playerLoc = getPlayerLoc(board);
            int y=playerLoc.first;
            int x=playerLoc.second;

            if(storageLocs.count({y,x}))
                board[y][x] = '.';
            else
                board[y][x] = ' ';

            for(int k=0;k<4;k++){

                int ty = y+dirt[k].first;
                int tx = x+dirt[k].second;

                if(inbox(ty, tx) && board[ty][tx]!='#'){

                    if(board[ty][tx]=='$'){
                        int by=ty+dirt[k].first;
                        int bx=tx+dirt[k].second;

                        if(moveBox(board, by, bx)){

                            char c1=board[ty][tx];
                            board[ty][tx]='@';

                            char c2=board[by][bx];
                            board[by][bx]='$';

                            if (c2!= '.' && detectAdjacentBoxDeadlock(board, by, bx)) {
                              board[ty][tx]=c1;
                              board[by][bx]=c2;
                              continue;
                            }

                            string tmp=encode(board);
                            board[ty][tx]=c1;
                            board[by][bx]=c2;

                            if(visited.count(tmp)) continue;

                            parent[tmp]=cur;
                            stk.push(tmp);
                        }

                    }else{

                        char c=board[ty][tx];
                        board[ty][tx]='@';
                        string tmp=encode(board);
                        board[ty][tx]=c;

                        if(visited.count(tmp)) continue;

                        parent[tmp]=cur;
                        stk.push(tmp);
                    }
                }
            }
        }

        void DFS(){

            stack<string> stk;
            stk.push(initState);

            unordered_set<string> visited;
            int steps=0;

            unordered_map<string, string> parent;
            parent[initState] = initState;

            while(!stk.empty()){

                int len=stk.size();
                steps++;

                for(int i=0;i<len;i++){

                    string cur=stk.top();
                    stk.pop();

                    //find the solution
                    if(finalStates.count(cur)){
                        print(parent, cur);
                        return ;
                    }

                    getNextStatesDFS(cur, stk, parent, visited);

                }
            }

            cout<<"No answer!"<<endl;
        }


        struct Node{
            int h; //manhattan distance between storages and boxes, smaller h has higher priority
            string state;
            Node(int h, string s):h(h), state(s){}
        };

        struct cmp{
            bool operator()(Node *a, Node *b){
                return a->h > b->h ;
            }
        };

        int getManhattanDist(string cur){

            vector<vector<char>> board = decode(cur);
            int h=0;

            for(int i=0;i<sizeH;i++){
                for(int j=0;j<sizeW;j++){
                    if(board[i][j]=='$'){

                        int min_h=INT_MAX;

                        for(auto s:storageLocs){
                            int sy=s.first;
                            int sx=s.second;
                            int tmp=0;

                            //box is on storage
                            if(sy==i && sx==j){
                                tmp -= 100;
                            }else{
                                tmp = abs(sy-i) + abs(sx-j);
                            }
                            min_h = min(tmp, min_h);
                        }

                        h+=min_h;
                    }
                }
            }

            return h;

        }

        void getNextStatesGreedy(Node *cur, priority_queue<Node*, vector<Node*>, cmp>& pq, unordered_map<string,string> &parent,  unordered_set<string> &visited){

            visited.insert(cur->state);
            vector<vector<char>> board = decode(cur->state);

            pair<int,int> playerLoc = getPlayerLoc(board);
            int y=playerLoc.first;
            int x=playerLoc.second;

            if(storageLocs.count({y,x}))
                board[y][x] = '.';
            else
                board[y][x] = ' ';

            for(int k=0;k<4;k++){

                int ty = y+dirt[k].first;
                int tx = x+dirt[k].second;

                if(inbox(ty, tx) && board[ty][tx]!='#'){

                    if(board[ty][tx]=='$'){
                        int by=ty+dirt[k].first;
                        int bx=tx+dirt[k].second;

                        if(moveBox(board, by, bx)){

                            char c1=board[ty][tx];
                            board[ty][tx]='@';

                            char c2=board[by][bx];
                            board[by][bx]='$';

                            if (c2!= '.' && detectAdjacentBoxDeadlock(board, by, bx)) {
                              board[ty][tx]=c1;
                              board[by][bx]=c2;
                              continue;
                            }

                            string tmp=encode(board);
                            board[ty][tx]=c1;
                            board[by][bx]=c2;

                            if(visited.count(tmp)) continue;

                            parent[tmp]=cur->state;

                            //Get new Manhanttan Distance when moving a box
                            int _h = getManhattanDist(tmp);
                            Node* next = new Node(_h, tmp);
                            pq.push(next);
                        }

                    }else{

                        char c=board[ty][tx];
                        board[ty][tx]='@';
                        string tmp=encode(board);
                        board[ty][tx]=c;

                        if(visited.count(tmp)) continue;

                        parent[tmp]=cur->state;
                        Node* next = new Node(cur->h, tmp);
                        pq.push(next);
                    }
                }
            }
        }


        void Greedy(){

            unordered_set<string> visited;

            unordered_map<string, string> parent;
            parent[initState] = initState;

            priority_queue<Node*, vector<Node*>, cmp> pq;
            int _h = getManhattanDist(initState);
            Node* head = new Node(_h, initState);
            pq.push(head);

            while(!pq.empty()){

                Node *cur=pq.top();
                pq.pop();

                if(finalStates.count(cur->state)){
                    print(parent, cur->state);
                    return ;
                }

                getNextStatesGreedy(cur, pq, parent, visited);

            }

            cout<<"No answer!"<<endl;

        }

/*
        void getNextStatesAstar(Node *cur, priority_queue<Node*, vector<Node*>, cmp>& pq, unordered_map<string,string> &parent,  unordered_set<string> &visited){

            visited.insert(cur->state);
            vector<vector<char>> board = decode(cur->state);

            pair<int,int> playerLoc = getPlayerLoc(board);
            int y=playerLoc.first;
            int x=playerLoc.second;

            if(storageLocs.count({y,x}))
                board[y][x] = '.';
            else
                board[y][x] = ' ';

            for(int k=0;k<4;k++){

                int ty = y+dirt[k].first;
                int tx = x+dirt[k].second;

                if(inbox(ty, tx) && board[ty][tx]!='#'){

                    if(board[ty][tx]=='$'){
                        int by=ty+dirt[k].first;
                        int bx=tx+dirt[k].second;

                        if(moveBox(board, by, bx)){

                            char c1=board[ty][tx];
                            board[ty][tx]='@';

                            char c2=board[by][bx];
                            board[by][bx]='$';

                            string tmp=encode(board);
                            board[ty][tx]=c1;
                            board[by][bx]=c2;

                            if(visited.count(tmp)) continue;

                            parent[tmp]=cur->state;

                            //Get new Manhanttan Distance when moving a box
                            int _h = getManhattanDist(tmp);
                            //f(n)=h(n)+g(n) the cost is 1
                            Node* next = new Node(_h+1, tmp);
                            pq.push(next);
                        }

                    }else{

                        char c=board[ty][tx];
                        board[ty][tx]='@';
                        string tmp=encode(board);
                        board[ty][tx]=c;

                        if(visited.count(tmp)) continue;

                        parent[tmp]=cur->state;
                        //f(n)=h(n)+g(n) the cost is 1
                        Node* next = new Node(cur->h+1, tmp);
                        pq.push(next);
                    }
                }
            }
        }


        void Astar(){

            unordered_set<string> visited;

            unordered_map<string, string> parent;
            parent[initState] = initState;

            priority_queue<Node*, vector<Node*>, cmp> pq;
            int _h = getManhattanDist(initState);
            Node* head = new Node(_h, initState);
            pq.push(head);

            while(!pq.empty()){

                Node *cur=pq.top();
                pq.pop();

                if(finalStates.count(cur->state)){
                    print(parent, cur->state);
                    return ;
                }

                getNextStatesAstar(cur, pq, parent, visited);

            }

            cout<<"No answer!"<<endl;

        }
*/


    public:
        Sokoban(string boardSize, string nWallSquares, string nBoxes, string nStorageLocations, string playerLoc){

            vector<vector<char>> board;

            initSize(board, boardSize);
            initWalls(board, nWallSquares);
            initStorageLoc(board, nStorageLocations);
            initBoxes(board, nBoxes);
            initPlayerLoc(board, playerLoc);
            initFinals(board);
            initBoundry(board);

            initState=encode(board);

            cout<<"Init: "<<endl;
            print(initState);

        }

        Sokoban(vector<vector<char>>& board){

            initFrom2DArray(board);
            initFinals(board);
            initBoundry(board);
            initState=encode(board);

            cout<<"Boundey: "<<endl;
            print(boundry);

            cout<<"Init: "<<endl;
            print(initState);
        }

        void print(unordered_map<string, string> &parent, string cur){

            vector<string> status{cur};

            while(parent[cur]!= cur){
                cur=parent[cur];
                status.push_back(cur);
            }

            reverse(status.begin(), status.end());

            for(string s:status){
                print(s);
            }

            cout <<"Steps : " << status.size()<<endl;

        }

        template <class T>
            void print(vector<vector<T>> &board){

                for(int i=0;i<sizeH;i++){
                    for(int j=0;j<sizeW;j++){
                        cout<<board[i][j]<<" ";
                    }
                    cout<<endl;
                }
                cout<<endl;

            }

        void print( string &s ){

            vector<vector<char>> board=decode(s);
            print(board);

        }

        void start(string i){

            //int i;
            //cout<<"Choose a method (1:Breadth-First Search 2:Depth-First Search 3:Greedy)"<<endl;
            //cin >> i;

            if(i.compare("1")==0){
                BFS();
            }else if(i.compare("2")==0){
                DFS();
            }else if(i.compare("3")==0){
                Greedy();
            }
        }
};


int main(int argc, char** argv){
/*
    Sokoban S(test1);
    S.start();
*/
    //while(1){
        if (argc != 3) return 0;

        string id;
        //cout << "Please enter a two digits number from 00 to 10:" << endl;
        //cin >> id;
        id = argv[1];

        string name = "sokoban" + id + ".txt";
        ifstream input("inputs/"+name);
        string boardSize, nWallSquares, nBoxes, nStorageLocations, playerLoc;

        getline(input, boardSize);
        getline(input, nWallSquares);
        getline(input, nBoxes);
        getline(input, nStorageLocations);
        getline(input, playerLoc);

        Sokoban S(boardSize, nWallSquares, nBoxes, nStorageLocations, playerLoc);
        S.start(argv[2]);

   // }
}
