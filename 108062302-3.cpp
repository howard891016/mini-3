#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>

struct Point {
    int x, y;
	Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> score_board;
std::vector<Point> valid_spots;
std::array<std::array<int, SIZE>, SIZE> board;

class OthelloBoard {
public:
     void operator =(OthelloBoard rhs)
     {
        for(int i = 0; i<SIZE; i++){
            for(int j = 0; j<SIZE; j++){

            board[i][j] = rhs.board[i][j];
            }
        }
        cur_player = rhs.cur_player;
        for(int i = 0; i<rhs.next_valid_spots.size(); i++){
            next_valid_spots[i] = rhs.next_valid_spots[i];
        }
    }
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    OthelloBoard() {
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }
        return true;
    }

};




int minimax(OthelloBoard state, int depth, int alpha, int beta)
{
    if(depth == 0 || state.next_valid_spots.size() == 0)
    {
        int val = 0;
        int new_score[8][8];
        for(int i = 0; i < SIZE ; i++)
        {
            for(int j = 0; j < SIZE ; j++)
            {
                new_score[i][j] = score_board[i][j];
            }
        }
        if(state.board[0][0] == player)
        {
            new_score[0][1] = abs(score_board[0][1]);
            new_score[1][1] = abs(score_board[1][1]);
            new_score[1][0] = abs(score_board[1][0]);
        }
        if(state.board[0][7] == player)
        {
            new_score[0][6] = abs(score_board[0][6]);
            new_score[1][6] = abs(score_board[1][6]);
            new_score[1][7] = abs(score_board[1][7]);
        }
        if(state.board[7][0] == player)
        {
            new_score[7][1] = abs(score_board[7][1]);
            new_score[6][1] = abs(score_board[6][2]);
            new_score[6][0] = abs(score_board[6][0]);
        }
        if(state.board[7][7] == player)
        {
            new_score[7][6] = abs(score_board[7][6]);
            new_score[6][6] = abs(score_board[6][6]);
            new_score[6][7] = abs(score_board[6][7]);
        }
        for(int i = 0;i < SIZE ; i++)
        {
            for(int j = 0;j<SIZE ;j++)
            {
                if(state.board[i][j] == player)
                {
                    val += new_score[i][j];
                }
            }
        }
        return val;
    }
    if(player == state.cur_player)
    {
        int bestval = -1000000;
        for(auto p : state.next_valid_spots)
        {
            OthelloBoard put = state;
            put.put_disc(p);
            int val = minimax(put, depth - 1, alpha, beta);
            bestval = std::max(bestval, val);
            alpha = std::max(alpha , bestval);
            if(alpha >= beta)
            {
                break;
            }
        }
        return bestval;
    }
    else
    {
        int bestval = 1000000;
        for(auto p : state.next_valid_spots)
        {
            OthelloBoard put = state;
            put.put_disc(p);
            int val = minimax(put, depth - 1, alpha, beta);
            bestval = std::min(bestval, val);
            beta = std::min(beta , bestval);
            if(alpha >= beta)
            {
                break;
            }
        }

        return bestval;
    }
}

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
			//board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        valid_spots.push_back({static_cast<float>(x), static_cast<float>(y)});
    }
}

void write_valid_spot(std::ofstream& fout) {
    srand(time(NULL));
	OthelloBoard state;
    for(int i = 0; i < SIZE; i++)
    {
        for(int j = 0; j < SIZE; j++)
        {
            state.board[i][j] = board[i][j];
        }
    }
    state.cur_player = player;
    state.next_valid_spots = valid_spots;
	int bestval = -1000;
	Point want;
	for(auto p : valid_spots)
	{
	    OthelloBoard put = state;
	    put.put_disc(p);
	    int val = minimax(put, 7, -2147483647, 2147483647);
	    if(val > bestval)
        {
            bestval = val;
            want = p;
        }
	}
    //Point p = valid_spots[index];
    // Remember to flush the output to ensure the last action is written to file.
    fout << want.x << " " << want.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    score_board[0] = {1000, -200, 50, 10, 10, 50, -200, 1000};
    score_board[1] = {-200, -300, 5, 5, 5, 5, -300, -200};
    score_board[2] = {50, 5, 1, 1, 1, 1, 5, 50};
    score_board[3] = {10, 5, 1, 1, 1, 1, 5, 10};
    score_board[4] = {10, 5, 1, 1, 1, 1, 5, 10};
    score_board[5] = {50, 5, 1, 1, 1, 1, 5, 50};
    score_board[6] = {-200, -300, 5, 5, 5, 5, -300, -200};
    score_board[7] = {1000, -200, 50, 10, 10, 50, -200, 1000};
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
