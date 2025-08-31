#pragma once
#include <SDL3/SDL.h>
#include "board.h"
#include "piece.h"
#include "randomiser.h"

struct RenderBoardInfo {
    int boardStartXPx, boardStartYPx;
    int boardWidthPx, boardHeightPx;
    int boardWidth, boardHeight;
};

class RenderHelper {
public:
    static void DrawRect(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color WHITE);
    static void DrawRectFilled(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color WHITE);
};

class TetrisRender {
public:
    TetrisRender(int width, int height, SDL_Renderer* renderer, SDL_Window* window);
    void RenderBoard(Board* board);
    
    void RenderPiece(Piece* piece);
    void RenderQueue(Randomiser* randomiser);
    void RenderHold(Piece* piece, bool alreadyHeld);
    void RenderGhost(Piece* piece);
    
private:
    int m_windowHeight, m_windowWidth;

    RenderBoardInfo m_renderInfo;
    Grid m_board;

    Piece m_currentPiece;
    Piece m_heldPiece;

    std::deque<Piece> m_queue;
    
    SDL_Renderer* m_renderer;
    SDL_Window* m_window;

private:
    void SetBoardInfo(Board* board);
    void RenderBoardOutline(Board* board);
    void RenderBoardPieces(Board* board);
    void RenderHoldPiece(Piece* piece, bool alreadyHeld);
    void RenderHoldFrame();
    void RenderQueueFrame();
    void RenderQueuePieces(Randomiser* randomiser);

};