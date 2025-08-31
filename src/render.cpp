#include "render.h"

void RenderHelper::DrawRect(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color) {
    static SDL_FRect tempRect;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    tempRect.x = x;
    tempRect.y = y;
    tempRect.w = w;
    tempRect.h = h;
    SDL_RenderRect(renderer, &tempRect);
}
void RenderHelper::DrawRectFilled(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color) {
    static SDL_FRect tempRect;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    tempRect.x = x;
    tempRect.y = y;
    tempRect.w = w;
    tempRect.h = h;
    SDL_RenderFillRect(renderer, &tempRect);
}

TetrisRender::TetrisRender(int width, int height, SDL_Renderer* renderer, SDL_Window* window)
    : m_windowWidth(width), m_windowHeight(height), m_renderer(renderer), m_window(window) 
{

}  

// High Level

void TetrisRender::RenderBoard(Board* board) {
    SetBoardInfo(board);

    RenderBoardOutline(board);
    RenderBoardPieces(board);
}

void TetrisRender::RenderHold(Piece* piece, bool alreadyHeld) {
    RenderHoldPiece(piece, alreadyHeld);
    RenderHoldFrame();
    return;
}

void TetrisRender::RenderQueue(Randomiser* randomiser) {
    m_queue = randomiser->GetQueue();
    RenderQueuePieces(randomiser);
}

void TetrisRender::SetBoardInfo(Board* board) {
    std::tie(m_renderInfo.boardWidth, m_renderInfo.boardHeight) = board->GetBoardInfo();
    m_renderInfo.boardStartXPx = m_windowWidth/2 - (BLOCK_WIDTH * m_renderInfo.boardWidth)/2;
    m_renderInfo.boardStartYPx = m_windowHeight - 90 - (BLOCK_HEIGHT * m_renderInfo.boardHeight);
    m_renderInfo.boardWidthPx = m_renderInfo.boardWidth * BLOCK_WIDTH;
    m_renderInfo.boardHeightPx = m_renderInfo.boardHeight * BLOCK_HEIGHT;
    m_board = board->GetGrid();
}

void TetrisRender::RenderBoardOutline(Board* board) {
    const SDL_Color WHITE = {255, 255, 255, 255};
    RenderHelper::DrawRect(m_renderer, m_renderInfo.boardStartXPx, m_renderInfo.boardStartYPx, m_renderInfo.boardWidthPx, m_renderInfo.boardHeightPx, WHITE);
    for (size_t row = 0; row < m_renderInfo.boardHeight; ++row) { //Grid Squares
        for (size_t col = 0; col < m_renderInfo.boardWidth; ++col) {
            //SDL_Log()
            const SDL_Color color = {160, 160, 160, 130};
            int posX = m_renderInfo.boardStartXPx + col*BLOCK_WIDTH;
            int posY = row*BLOCK_HEIGHT + m_renderInfo.boardStartYPx;
            RenderHelper::DrawRect(m_renderer, posX, posY, BLOCK_WIDTH, BLOCK_HEIGHT, color);
        }
    }
}

void TetrisRender::RenderBoardPieces(Board* board) {
    for (size_t row = 0; row < m_board.size(); ++row) { //Actual pieces
        for (size_t col = 0; col < m_board[row].size(); ++col) {
            const Block& block = m_board[row][col];
            if (block.color.a == SDL_ALPHA_TRANSPARENT) continue;   
            int posX = m_renderInfo.boardStartXPx + block.x*BLOCK_WIDTH;
            int posY = m_renderInfo.boardStartYPx + (m_renderInfo.boardHeight - 1 - block.y) * BLOCK_HEIGHT;
            // SDL_Log("StartPos: %d, %d", boardStartX, boardStartY);
            // SDL_Log("Position: %d, %d", posX, posY);
            // SDL_Log("Indice: %d, %d", block.x, block.y);
            //SDL_Log()
            RenderHelper::DrawRectFilled(m_renderer, posX, posY, BLOCK_WIDTH, BLOCK_HEIGHT, block.color);
        }
    }
}

void TetrisRender::RenderHoldFrame() {
    const SDL_Color WHITE = {255, 255, 255, 255};
    const int frameWidth = 200, frameHeight = 150;
    //Render some text maybe
    RenderHelper::DrawRect(m_renderer, m_renderInfo.boardStartXPx - 290, m_renderInfo.boardStartYPx + 40 , frameWidth, frameHeight, WHITE);
}

void TetrisRender::RenderHoldPiece(Piece* heldPiece, bool alreadyHeld) {
    Tetrimino m_piece = heldPiece->GetPiece();
    heldPiece->Reset(-5, 16);
    for (Block block: m_piece) {
        if (alreadyHeld) {block.color.a -= 40;};
        int posX = m_renderInfo.boardStartXPx + block.x*BLOCK_WIDTH;
        int posY = m_renderInfo.boardStartYPx + (m_renderInfo.boardHeight - 1 - block.y) * BLOCK_HEIGHT;
        RenderHelper::DrawRectFilled(m_renderer, posX, posY, BLOCK_WIDTH, BLOCK_HEIGHT, block.color);
    }
}

void TetrisRender::RenderQueuePieces(Randomiser* randomiser) {
    const int centerX = m_renderInfo.boardWidth + 4;
    const int boardEndPx = m_renderInfo.boardStartXPx + m_renderInfo.boardWidthPx;
    const float offsetYBlocks = 3;
    for (int index = 0; index < m_queue.size(); index++) {
        m_queue[index].Reset(centerX, 18 - index*offsetYBlocks);
        //RenderHelper::DrawRectFilled(m_renderer, boardEndPx + centerX * BLOCK_WIDTH);
        
        for (Block block: m_queue[index].GetPiece()) {
            int posX = m_renderInfo.boardStartXPx + block.x*BLOCK_WIDTH;
            int posY = m_renderInfo.boardStartYPx + (m_renderInfo.boardHeight - 1 - block.y) * BLOCK_HEIGHT;
            RenderHelper::DrawRectFilled(m_renderer, posX, posY, BLOCK_WIDTH, BLOCK_HEIGHT, block.color);
        }
    }
}

void TetrisRender::RenderGhost(Piece* piece) { 
    Tetrimino ghost = piece->GetGhost();
    for (Block block: ghost) {
        block.color.a = SDL_ALPHA_TRANSPARENT + 100;
        int posX = m_renderInfo.boardStartXPx + block.x*BLOCK_WIDTH;
        int posY = m_renderInfo.boardStartYPx + (m_renderInfo.boardHeight - 1 - block.y) * BLOCK_HEIGHT;
        RenderHelper::DrawRectFilled(m_renderer, posX, posY, BLOCK_WIDTH, BLOCK_HEIGHT, block.color);
    }
}