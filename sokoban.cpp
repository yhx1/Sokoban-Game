#include<iostream>
#include<vector>
#include<string>
#include<unordered_set>
#include<unordered_map>
#include<sstream>
#include<utility>
#include<queue>

using namespace std;

#include "test.h"


class Sokoban{

    private:

        int sizeH;
        int sizeW;
        int walls; //number of walls
        int boxes; //number of boxes
        int storages; //number of storages

        string initState;
        unordered_set<string> finalStates; 

        vector<vector<bool>> boundry;

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
                board[0][i] = 'W';
                board[sizeH-1][i] = 'W';
            }

            for(int i=0;i<sizeH;i++){
                board[i][0] = 'W';
                board[i][sizeW-1] = 'W';
            }

        }

        void initWalls(vector<vector<char>> &board, string nWallSquares){

            stringstream ss;
            ss<<nWallSquares;

            ss>>walls;

            while(!ss.eof()){
                int x,y;
                ss>>x;
                x-=1;
                ss>>y;
                y-=1;
                board[y][x] = 'W';
            }

            //set walls to bound the box if not given by input
            //setWalls(board);

        }

        void initBoxes(vector<vector<char>> &board, string nBoxes){

            stringstream ss;
            ss<<nBoxes;

            ss>>boxes;

            while(!ss.eof()){
                int x,y;
                ss>>x;
                x-=1;
                ss>>y;
                y-=1;
                board[y][x] = 'B';
            }
        }

        void initStorageLoc(vector<vector<char>>&board, string nStorageLocations){

            stringstream ss;
            ss<<nStorageLocations;

            ss>>storages;

            while(!ss.eof()){
                int x,y;
                ss>>x;
                x-=1;
                ss>>y;
                y-=1;
                board[y][x] = 'S';
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

            board[y][x] = 'P';

        }

        void initBoundry(vector<vector<char>> &board){

            boundry.resize(sizeH, vector<bool>(sizeW,true));

            //change corners to false(unreachable)
            for(int i=0; i<sizeH; i++){
                for(int j=0; j<sizeW; j++){

                    if(board[i][j]=='W'){
                        boundry[i][j] = false;

                    }else if(board[i][j]==' ' || board[i][j]=='P'){    

                        int cnt_x=0;
                        int cnt_y=0;

                        for(int k=0;k<4;k++){

                            int ty=i+dirt[k].first;
                            int tx=j+dirt[k].second;

                            if(inbox(ty, tx) && board[ty][tx]=='W'){
                                if(k==0 || k==1) cnt_y++;
                                else cnt_x++;
                            }
                        }

                        if(cnt_y>=1 && cnt_x>=1)
                            boundry[i][j] = false;
                    }
                }
            }
        }

        void initFrom2DArray(vector<vector<char>> &board){
            
            sizeH=board.size();
            sizeW=board[0].size();

            for(int i=0;i<sizeH;i++){
                for(int j=0;j<sizeW;j++){
                    if(board[i][j]=='W')
                        walls++;
                    else if(board[i][j]=='B')
                        boxes++;
                    else if(board[i][j]=='S')
                        storages++;
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
                    board[ty][tx] = 'P';
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

                    if(board[i][j]=='S')
                        board[i][j] = 'B';
                    else if(board[i][j]=='B')
                        board[i][j] = ' ';
                    else if(board[i][j]=='P')
                        board[i][j] = ' ';
                }
            }

            for(int i=0;i<sizeH;i++){
                for(int j=0;j<sizeW;j++){

                    if(board[i][j]=='B'){
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

                    if(board[i][j]=='P'){
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

            if(!inbox(by, bx) || board[by][bx]=='W' || board[by][bx]=='B' || !validPos(by, bx))
                return false;

            return true;
        }

        void getNextStates(vector<vector<char>> board, queue<string>& q, unordered_map<string,string> &parent,  unordered_set<string> &visited, pair<int,int> playerLoc){

            int y=playerLoc.first;
            int x=playerLoc.second;
            string cur=encode(board);
            board[y][x] = ' ';

            for(int k=0;k<4;k++){

                int ty = y+dirt[k].first;
                int tx = x+dirt[k].second;

                if(inbox(ty, tx) && board[ty][tx]!='W'){

                    if(board[ty][tx]=='B'){
                        int by=ty+dirt[k].first;
                        int bx=tx+dirt[k].second;

                        if(moveBox(board, by, bx)){

                            char c1=board[ty][tx];
                            board[ty][tx]='P';

                            char c2=board[by][bx];
                            board[by][bx]='B';

                            string tmp=encode(board);
                            board[ty][tx]=c1;
                            board[by][bx]=c2;

                            if(visited.count(tmp)) continue;

                            parent[tmp]=cur;
                            q.push(tmp);
                        }

                    }else{

                        board[ty][tx]='P';
                        string tmp=encode(board);
                        board[ty][tx]=' ';

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

                    visited.insert(cur);

                    vector<vector<char>> board = decode(cur);
                    pair<int,int> playerLoc = getPlayerLoc(board);

                    getNextStates(board, q, parent, visited, playerLoc);

                }        
            }

            cout<<"No answer!"<<endl;
        }


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

        void start(){
            BFS(); 
        }
};


int main(){
    /*
       Sokoban S(test3);
       S.start();
       */

    Sokoban S(test2);
    S.start();

    /*    
          Sokoban S(test4);
          S.start();
          */
    /*
       Sokoban S(test1);
       S.start();
       */    
}
