#pragma once
#include <wx/dc.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/window.h>
#include <wx/bitmap.h>
#include <wx/log.h>
#include <stack>
#include <iostream>

#include "game.h"

class canvas
{
protected:
    wxBitmap stone[2];
    wxBitmap palete;
    std::stack<game_board> redos;
    // 棋盘和初始界面
    game_board* can;

public:
    canvas();
    std::stack<game_board> board;
    void set_game_board(const game_board& board);
    game_board* get_current_board();
    // 通过指针初始化
    void set_game_board(game_board* board) {
        can = board;
    }

    // 修改方法，不再创建局部变量 can，而是直接操作成员变量 can
    void add_dead_marker(int x, int y) {
        if (!can) {
            wxLogError("Game board pointer is null!");
            return;
        }
        wxLogDebug("add点击位置: (%d, %d)", x, y);
        if (x >= 0 && x < 21 && y >= 0 && y < 21) {
            can->dead_stones[x][y] = true;  // 修改 frame.cpp 中的 can
        }
    }

    void remove_dead_marker(int x, int y) {
        if (!can) {
            wxLogError("Game board pointer is null!");
            return;
        }
        wxLogDebug("rem点击位置: (%d, %d)", x, y);
        if (x >= 0 && x < 21 && y >= 0 && y < 21) {
            can->dead_stones[x][y] = false;  // 修改 frame.cpp 中的 can
        }
    }

    void realise(wxDC& target);

    game_board& get_board() {
        return board.top();
    }

    bool can_place(int colour, int x, int y) {
        return board.top().can_place(colour, x, y);
    }

    bool is_dead(int x, int y) {
        if (!can) {
            wxLogError("Game board pointer is null!");
            return false;
        }
        wxLogDebug("is dead 点击位置: (%d, %d)", x, y);
        return can->dead_stones[x][y];
    }

    void place(int colour, int x, int y) {
        game_board tmp{board.top()};
        tmp.place(colour, x, y);
        tmp.record_step(x, y);
        board.push(tmp);
        while (!redos.empty()) {
            redos.pop();
        }
    }

    void load(std::istream& input);
    void save(std::ostream& output);

    void clear() {
        while (!redos.empty()) {
            redos.pop();
        }
        while (!board.empty()) {
            board.pop();
        }
        board.push(game_board());
    }

    bool can_undo() {
        return board.size() > 1;
    }

    void undo() {
        if (can_undo()) {
            redos.push(board.top());
            board.pop();
        } else {
            wxLogError("Cannot undo, no more moves to undo.");
        }
    }

    bool can_redo() {
        return !redos.empty();
    }

    void redo() {
        if (can_redo()) {
            board.push(redos.top());
            redos.pop();
        } else {
            wxLogError("Cannot redo, no moves to redo.");
        }
    }

private:
    void selectBackgroundFromPreset();
    void preparePalete();
};