#include <stdio.h>
#include <string>
#include <ctime>
#include <iostream>
#include <SDL_ttf.h>
#include <SDL.h>
#include <SDL_image.h>
#include <algorithm>
#include <vector>
#include <random>
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 660;
const int IMAGE_WIDTH = 50;
const int IMAGE_HEIGHT = 50;
const int IMAGE_INIT = 30;
const int MAGNIFICATION = 4;
const int IMAGE_NUM = 44;

int imagearray[14][14] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    };

bool init(SDL_Window** window, SDL_Renderer** renderer, TTF_Font** font);
void close(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, TTF_Font* font);

// ボタンの構造体
struct Button {
    SDL_Rect rect;
    SDL_Color color;
    SDL_Color hoverColor;
    SDL_Color currentColor;
    std::string text;
    TTF_Font* font;
    SDL_Texture* texture;
};

// ボタンの初期化
bool initButton(Button& button, SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, int w, int h) {
    button.rect = { x, y, w, h };
    button.color = { 100, 100, 100, 255 }; // 白
    button.hoverColor = { 130, 130, 130, 255 }; // グレー
    button.currentColor = button.color;
    button.text = text;
    button.font = font;

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), { 255, 255, 255, 255 });
    if (textSurface == nullptr) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    button.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    if (button.texture == nullptr) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

// ボタンの描画
void renderButton(Button& button, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, button.currentColor.r, button.currentColor.g, button.currentColor.b, button.currentColor.a);
    SDL_RenderFillRect(renderer, &button.rect);

    int textW, textH;
    SDL_QueryTexture(button.texture, NULL, NULL, &textW, &textH);
    SDL_Rect textRect = { button.rect.x + (button.rect.w - textW) / 2, button.rect.y + (button.rect.h - textH) / 2, textW, textH };
    SDL_RenderCopy(renderer, button.texture, NULL, &textRect);
}

// マウスイベントの処理
bool handleMouseEvent(Button& button, SDL_Event* e) {
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        bool inside = true;

        if (x < button.rect.x) {
            inside = false;
        }
        else if (x > button.rect.x + button.rect.w) {
            inside = false;
        }
        else if (y < button.rect.y) {
            inside = false;
        }
        else if (y > button.rect.y + button.rect.h) {
            inside = false;
        }

        if (!inside) {
            button.currentColor = button.color;
        }
        else {
            button.currentColor = button.hoverColor;

            if (e->type == SDL_MOUSEBUTTONDOWN) {
                return true;
            }
        }
    }
    return false;
}
bool subWindow(int num) {
    SDL_Window* timeUpWindow = SDL_CreateWindow("Hangout", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 300, 200, SDL_WINDOW_SHOWN);
    if (timeUpWindow == nullptr) {
        std::cerr << "Unable to create time up window! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Renderer* timeUpRenderer = SDL_CreateRenderer(timeUpWindow, -1, SDL_RENDERER_ACCELERATED);
    if (timeUpRenderer == nullptr) {
        std::cerr << "Unable to create renderer for time up window! SDL Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(timeUpWindow);
        return false;
    }

    SDL_SetRenderDrawColor(timeUpRenderer, 255, 255, 255, 255);
    SDL_RenderClear(timeUpRenderer);

    TTF_Font* font = TTF_OpenFont("fonts/arial.ttf", 24);
    if (font == nullptr) {
        std::cerr << "Unable to open font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(timeUpRenderer);
        SDL_DestroyWindow(timeUpWindow);
        return false;
    }

    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* textSurface = NULL;
    if (num == 0) {
        textSurface = TTF_RenderText_Solid(font, "Time is up!", textColor);
    }
    else if (num == 1) {
        textSurface = TTF_RenderText_Solid(font, "Clear!", textColor);
    }
    if (textSurface == nullptr) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        TTF_CloseFont(font);
        SDL_DestroyRenderer(timeUpRenderer);
        SDL_DestroyWindow(timeUpWindow);
        return false;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(timeUpRenderer, textSurface);
    SDL_FreeSurface(textSurface);
    if (textTexture == nullptr) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        TTF_CloseFont(font);
        SDL_DestroyRenderer(timeUpRenderer);
        SDL_DestroyWindow(timeUpWindow);
        return false;
    }

    int textW, textH;
    SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
    SDL_Rect textRect = { (300 - textW) / 2, (100 - textH) / 2, textW, textH };
    SDL_RenderCopy(timeUpRenderer, textTexture, NULL, &textRect);
    SDL_RenderPresent(timeUpRenderer);

    Button button;
    if (!initButton(button, timeUpRenderer, font, "NEXT", 75, 100, 150, 50)) {
        TTF_CloseFont(font);
        SDL_DestroyRenderer(timeUpRenderer);
        SDL_DestroyWindow(timeUpWindow);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    SDL_Event e;
    bool quit = false;
    bool buttonClicked = false;
    while (!quit) {
        SDL_SetRenderDrawColor(timeUpRenderer, 255, 255, 255, 255);
        SDL_RenderClear(timeUpRenderer);

        SDL_RenderCopy(timeUpRenderer, textTexture, NULL, &textRect);
        renderButton(button, timeUpRenderer); // イベントループ内でボタンを描画

        SDL_RenderPresent(timeUpRenderer);
        if (SDL_WaitEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                if (e.window.windowID == SDL_GetWindowID(timeUpWindow)) {
                    quit = true;
                }
            }
            else if (handleMouseEvent(button, &e)) {
                if (e.window.windowID == SDL_GetWindowID(timeUpWindow)) {
                    buttonClicked = true;
                    quit = true;
                }
            }
        }
    }

    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(timeUpRenderer);
    SDL_DestroyWindow(timeUpWindow);
    return buttonClicked;
}
void Renderinit1(SDL_Renderer* renderer, SDL_Texture* textures[IMAGE_NUM]) {
    std::vector<int> Elements;
    int imagearray_temp[144] = {
        1,1,1,1,1,1,1,1,2,2,2,2,
        2,2,2,2,3,3,3,3,3,3,3,3,
        4,4,4,4,4,4,4,4,5,5,5,5,
        5,5,5,5,6,6,6,6,6,6,6,6,
        7,7,7,7,7,7,7,7,8,8,8,8,
        8,8,8,8,9,9,9,9,9,9,9,9,
        10,10,10,10,10,10,10,10,11,11,11,11,
        11,11,11,11,12,12,12,12,12,12,12,12,
        13,13,13,13,13,13,13,13,14,14,14,14,
        14,14,15,15,15,15,15,15,16,16,16,16,
        16,16,17,17,17,17,17,17,18,18,18,18,
        18,18,19,19,19,19,19,19,20,20,20,20
    };

    // 要素を収集
    for (int i = 0; i < 144; ++i) {
        Elements.push_back(imagearray_temp[i]);
    }
    
    // シャッフル
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(Elements.begin(), Elements.end(), g);

    int index = 0;
    // 画像を縦12枚、横12枚に並べて描画
    for (int row = 0; row < 12; ++row) {
        for (int col = 0; col < 12; ++col) {
            imagearray[row + 1][col + 1] = Elements[index++];
            SDL_Rect dstRect = { col * IMAGE_WIDTH + 30, row * IMAGE_HEIGHT + 30 , IMAGE_WIDTH - 2, IMAGE_HEIGHT - 2 };
            if (Elements[index - 1] != 0) {
                SDL_RenderCopy(renderer, textures[Elements[index - 1] - 1], NULL, &dstRect);
            }
        }
    }
}

void Renderinit23(SDL_Renderer* renderer, SDL_Texture* textures[IMAGE_NUM]) {
    std::vector<int> Elements;
    int imagearray_temp[144] = {
        1,1,1,1,1,1,2,2,2,2,2,2,
		3,3,3,3,3,3,4,4,4,4,4,4,
		5,5,5,5,5,5,6,6,6,6,6,6,
		7,7,7,7,7,7,8,8,8,8,8,8,
		9,9,9,9,9,9,10,10,10,10,10,10,
		11,11,11,11,11,11,12,12,12,12,12,12,
		13,13,13,13,13,13,14,14,14,14,14,14,
		15,15,15,15,15,15,16,16,16,16,16,16,
		17,17,17,17,18,18,18,18,19,19,19,19,
		20,20,20,20,21,21,21,21,22,22,22,22,
		23,23,23,23,24,24,24,24,25,25,25,25,
		26,26,26,26,27,27,27,27,28,28,28,28
    };

    // 要素を収集
    for (int i = 0; i < 144; ++i) {
        Elements.push_back(imagearray_temp[i]);
    }

    // シャッフル
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(Elements.begin(), Elements.end(), g);

    int index = 0;
    // 画像を縦12枚、横12枚に並べて描画
    for (int row = 0; row < 12; ++row) {
        for (int col = 0; col < 12; ++col) {
            imagearray[row + 1][col + 1] = Elements[index++];
            SDL_Rect dstRect = { col * IMAGE_WIDTH + 30, row * IMAGE_HEIGHT + 30 , IMAGE_WIDTH - 2, IMAGE_HEIGHT - 2 };
            if (Elements[index - 1] != 0) {
                SDL_RenderCopy(renderer, textures[Elements[index - 1] - 1], NULL, &dstRect);
            }
        }
    }
}

void Renderinit45(SDL_Renderer* renderer, SDL_Texture* textures[IMAGE_NUM]) {
    std::vector<int> Elements;
    int imagearray_temp[144] = {
        1,1,1,1,2,2,2,2,3,3,3,3,
		4,4,4,4,5,5,5,5,6,6,6,6,
		7,7,7,7,8,8,8,8,9,9,9,9,
		10,10,10,10,11,11,11,11,12,12,12,12,
		13,13,13,13,14,14,14,14,15,15,15,15,
		16,16,16,16,17,17,17,17,18,18,18,18,
		19,19,19,19,20,20,20,20,21,21,21,21,
		22,22,22,22,23,23,23,23,24,24,24,24,
		25,25,25,25,26,26,26,26,27,27,27,27,
		28,28,28,28,29,29,29,29,30,30,30,30,
		31,31,31,31,32,32,32,32,33,33,33,33,
		34,34,34,34,35,35,35,35,36,36,36,36
    };

    // 要素を収集
    for (int i = 0; i < 144; ++i) {
        Elements.push_back(imagearray_temp[i]);
    }

    // シャッフル
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(Elements.begin(), Elements.end(), g);

    int index = 0;
    // 画像を縦12枚、横12枚に並べて描画
    for (int row = 0; row < 12; ++row) {
        for (int col = 0; col < 12; ++col) {
            imagearray[row + 1][col + 1] = Elements[index++];
            SDL_Rect dstRect = { col * IMAGE_WIDTH + 30, row * IMAGE_HEIGHT + 30 , IMAGE_WIDTH - 2, IMAGE_HEIGHT - 2 };
            if (Elements[index - 1] != 0) {
                SDL_RenderCopy(renderer, textures[Elements[index - 1] - 1], NULL, &dstRect);
            }
        }
    }
}

void Renderinit6(SDL_Renderer* renderer, SDL_Texture* textures[IMAGE_NUM]) {
    std::vector<int> Elements;
    int imagearray_temp[144] = {
        1,1,1,1,2,2,2,2,3,3,3,3,
        4,4,4,4,5,5,5,5,6,6,6,6,
        7,7,7,7,8,8,8,8,9,9,9,9,
        10,10,10,10,11,11,11,11,12,12,12,12,
        13,13,13,13,14,14,14,14,15,15,15,15,
        16,16,16,16,17,17,17,17,18,18,18,18,
        19,19,19,19,20,20,20,20,21,21,21,21,
		22,22,22,22,23,23,23,23,24,24,24,24,
		25,25,25,25,26,26,26,26,27,27,27,27,
		28,28,28,28,29,29,30,30,31,31,32,32,
		33,33,34,34,35,35,36,36,37,37,38,38,
		39,39,40,40,41,41,42,42,43,43,44,44
    };

    // 要素を収集
    for (int i = 0; i < 144; ++i) {
        Elements.push_back(imagearray_temp[i]);
    }

    // シャッフル
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(Elements.begin(), Elements.end(), g);

    int index = 0;
    // 画像を縦12枚、横12枚に並べて描画
    for (int row = 0; row < 12; ++row) {
        for (int col = 0; col < 12; ++col) {
            imagearray[row + 1][col + 1] = Elements[index++];
            SDL_Rect dstRect = { col * IMAGE_WIDTH + 30, row * IMAGE_HEIGHT + 30 , IMAGE_WIDTH - 2, IMAGE_HEIGHT - 2 };
            if (Elements[index - 1] != 0) {
                SDL_RenderCopy(renderer, textures[Elements[index - 1] - 1], NULL, &dstRect);
            }
        }
    }
}

void restart(SDL_Renderer* renderer, SDL_Texture* textures[IMAGE_NUM]) {

    // 画像を縦12枚、横12枚に並べて描画
    for (int row = 0; row < 12; ++row) {
        for (int col = 0; col < 12; ++col) {
            int index;           
            index = imagearray[row + 1][col + 1];
            SDL_Rect dstRect = { col * IMAGE_WIDTH + 30, row * IMAGE_HEIGHT + 30 , IMAGE_WIDTH - 2, IMAGE_HEIGHT - 2 };
            if (index != 0) {
                SDL_RenderCopy(renderer, textures[index - 1], NULL, &dstRect);
            }
        }
    }
}

void initializeArrays(int row_array[14][2], int col_array[14][2]) {
    for (int i = 0; i < 14; ++i) {
        row_array[i][0] = -1;
        col_array[i][0] = -1;
        row_array[i][1] = -1;
        col_array[i][1] = -1;
    }
}

void checkRowDirection(int row, int col, int row_array[14][2], int imagearray[14][14]) {
    int temp_row = row + 1;
    int temp_col = col + 1;
    int i, j;

    for (i = 0; i < 14; ++i) {
        row_array[i][1] = temp_row;
        row_array[i][0] = temp_col;
        temp_row++;
        if (temp_row > 13 || imagearray[temp_row][temp_col] != 0) {
            break;
        }
    }
    temp_row = row;
    for (j = ++i; j < 14; j++) {
        if (temp_row < 0 || imagearray[temp_row][temp_col] != 0) {
            break;
        }
        row_array[j][1] = temp_row;
        row_array[j][0] = temp_col;
        temp_row--;
    }
}

void checkColDirection(int row, int col, int col_array[14][2], int imagearray[14][14]) {
    int temp_row = row + 1;
    int temp_col = col + 1;
    int i, j;

    for (i = 0; i < 14; i++) {
        col_array[i][1] = temp_row;
        col_array[i][0] = temp_col;
        temp_col++;
        if (temp_col > 13 || imagearray[temp_row][temp_col] != 0) {
            break;
        }
    }
    temp_col = col;
    for (j = ++i; j < 14; j++) {
        if (temp_col < 0 || imagearray[temp_row][temp_col] != 0) {
            break;
        }
        col_array[j][1] = temp_row;
        col_array[j][0] = temp_col;
        temp_col--;
    }
}

void processArrays(int row, int col, int row_array[14][2], int col_array[14][2], int imagearray[14][14]) {
    initializeArrays(row_array, col_array);
    checkRowDirection(row, col, row_array, imagearray);
    checkColDirection(row, col, col_array, imagearray);
}

int checkArrays(int prev_row, int prev_col, int row, int col, int imagearray[14][14]) {
    int col_array[14][2];
    int row_array[14][2];
    int prev_col_array[14][2];
    int prev_row_array[14][2];
    int flag = 0;

    processArrays(prev_row, prev_col, prev_row_array, prev_col_array, imagearray);
    processArrays(row, col, row_array, col_array, imagearray);

    for (int i = 0; i < 14; i++) {
        for (int j = 0; j < 14; j++) {
            if ((prev_row_array[i][0] == col_array[j][0] && prev_row_array[i][1] == col_array[j][1] && prev_row_array[i][0] != -1) ||
                (prev_col_array[i][0] == row_array[j][0] && prev_col_array[i][1] == row_array[j][1] && prev_col_array[i][0] != -1)) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        }
    }

    if (prev_col == col + 1 && prev_row == row ||
        prev_col == col - 1 && prev_row == row ||
        prev_row == row + 1 && prev_col == col ||
        prev_row == row - 1 && prev_col == col) {
        flag = 1;
    }

    if (flag != 1) {
        for (int i = 0; i < 14; i++) {
            for (int j = 0; j < 14; j++) {
                if (prev_col_array[i][0] == col_array[j][0] && prev_col_array[i][0] != -1) {
                    int start_row = min(prev_col_array[i][1], col_array[j][1]);
                    int end_row = max(prev_col_array[i][1], col_array[j][1]);
                    for (int k = ++start_row; k < end_row; k++) {
                        if (imagearray[k][prev_col_array[i][0]] != 0) {
                            flag = 2;
                            break;
                        }
                    }
                    if (flag == 2) {
                        flag = 0;
                    }
                    else {
                        flag = 1;
                    }
                }
            }
            if (flag == 1) {
                break;
            }
        }
    }

    if (flag != 1) {
        for (int i = 0; i < 14; i++) {
            for (int j = 0; j < 14; j++) {
                if (prev_row_array[i][1] == row_array[j][1] && prev_row_array[i][1] != -1) {
                    int start_col = min(prev_row_array[i][0], row_array[j][0]);
                    int end_col = max(prev_row_array[i][0], row_array[j][0]);
                    for (int k = ++start_col; k < end_col; k++) {
                        if (imagearray[prev_row_array[i][1]][k] != 0) {
                            flag = 2;
                            break;
                        }
                    }
                    if (flag == 2) {
                        flag = 0;
                    }
                    else {
                        flag = 1;
                    }
                }
            }
            if (flag == 1) {
                break;
            }
        }
    }

    return flag;
}

// 0以外の要素をシャッフルする関数
void shuffleNonZeroElements(int array[14][14]) {
    std::vector<int> nonZeroElements;

    // 0以外の要素を収集
    for (int i = 0; i < 14; ++i) {
        for (int j = 0; j < 14; ++j) {
            if (array[i][j] != 0) {
                nonZeroElements.push_back(array[i][j]);
            }
        }
    }

    // シャッフル
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(nonZeroElements.begin(), nonZeroElements.end(), g);

    // シャッフルされた要素を元の配列に戻す
    int index = 0;
    for (int i = 0; i < 14; ++i) {
        for (int j = 0; j < 14; ++j) {
            if (array[i][j] != 0) {
                array[i][j] = nonZeroElements[index++];
            }
        }
    }
}

bool findPair(int imagearray[14][14], int pair[2][2], SDL_Renderer* renderer, SDL_Texture* textures[IMAGE_NUM]) {
    bool checkpair = false;
    while (!checkpair) {
        for (int i = 0; i < 144; i++) {
            for (int j = i + 1; j < 144; j++) {
                if (imagearray[i / 12 + 1][i % 12 + 1] == imagearray[j / 12 + 1][j % 12 + 1] && imagearray[i / 12 + 1][i % 12 + 1] != 0) {
                    if (checkArrays(i / 12, i % 12, j / 12, j % 12, imagearray) != 1) {
                        checkpair = false;
                    }
                    else {
                        pair[0][0] = i % 12;
                        pair[0][1] = i / 12;
                        pair[1][0] = j % 12;
                        pair[1][1] = j / 12;
                        checkpair = true;
                        break;
                    }
                }
            }
            if (checkpair) {
                break;
            }
        }
        if (!checkpair) {
            shuffleNonZeroElements(imagearray);
            restart(renderer, textures);
        }
    }
    return checkpair;
}

void renderStage(SDL_Renderer* renderer, TTF_Font* font, int stage) {
    SDL_Color textColor = { 0, 0, 0, 255 }; // テキストの色
    std::string stageText = "Stage: " + std::to_string(stage);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, stageText.c_str(), textColor);
    if (textSurface == nullptr) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    if (textTexture == nullptr) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Rect textRect2 = { 635, 400, 160, 50 }; // テキストの描画領域
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect2);
    SDL_DestroyTexture(textTexture);
}

void removeAndShift(int imagearray[14][14], int temp_selected_index[2], int prev_selected_index[2]) {
    int col = temp_selected_index[0] + 1;
    int startRow1 = temp_selected_index[1] + 1;
    int startRow2 = prev_selected_index[1] + 1;

    if (temp_selected_index[0] == prev_selected_index[0]) {
        // 同じ列の場合
        int minRow = std::min(startRow1, startRow2);
        int maxRow = std::max(startRow1, startRow2);

        // 2つの要素を削除して詰める
        for (int i = minRow; i < 13; i++) {
            imagearray[i][col] = imagearray[i + 1][col];
        }
        for (int i = maxRow - 1; i < 13; i++) {
            imagearray[i][col] = imagearray[i + 1][col];
        }
    }
    else {
        // 異なる列の場合
        for (int i = startRow1; i < 13; i++) {
            imagearray[i][col] = imagearray[i + 1][col];
        }

        col = prev_selected_index[0] + 1;
        for (int i = startRow2; i < 13; i++) {
            imagearray[i][col] = imagearray[i + 1][col];
        }
    }
}

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(0))); // 追加: 乱数生成器をシード

    const int IMAGE_WIDTH_SENTER = IMAGE_WIDTH * MAGNIFICATION / 2;
    const int IMAGE_HEIGHT_SENTER = IMAGE_HEIGHT * MAGNIFICATION / 2;

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    TTF_Font* font = NULL;


    // ウィンドウの位置
    SDL_Point window_position = {
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED
    };

    // ウィンドウのサイズ
    SDL_Point window_size = { SCREEN_WIDTH, SCREEN_HEIGHT };

    // マウスの座標    
    SDL_Point mouse_position = { IMAGE_WIDTH_SENTER, IMAGE_WIDTH_SENTER };

    if (!init(&window, &renderer, &font)) {
        printf("Failed to initialize!\n");
        return 1;
    }

    Button button1;
    if (!initButton(button1, renderer, font, "START/STOP", 635, 200, 155, 50)) {
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    Button button2;
    if (!initButton(button2, renderer, font, "HINT", 635, 300, 155, 50)) {
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // ボタンの描画
    renderButton(button1, renderer);
    renderButton(button2, renderer);


    // 画像の読み込み


    // 透過色の設定
    //SDL_SetColorKey(image1, SDL_TRUE, SDL_MapRGB(image1->format, 255, 0, 255));
    // 画像の読み込みとテクスチャの作成
    SDL_Surface* images[IMAGE_NUM];
    SDL_Texture* textures[IMAGE_NUM];
    char filename[15];

    for (int i = 0; i < IMAGE_NUM; ++i) {
        sprintf_s(filename, "images/%d.bmp", i + 1);
        images[i] = IMG_Load(filename);
        if (!images[i]) {
            printf("IMG_Load: %s\n", IMG_GetError());
            return 1;
        }
        textures[i] = SDL_CreateTextureFromSurface(renderer, images[i]);
        if (!textures[i]) {
            printf("SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
            return 1;
        }
        SDL_FreeSurface(images[i]);
    }
    SDL_Surface* images_s[IMAGE_NUM];
    SDL_Texture* textures_s[IMAGE_NUM];
    for (int i = 0; i < IMAGE_NUM; ++i) {
        sprintf_s(filename, "images/%ds.bmp", i + 1);
        images_s[i] = IMG_Load(filename);
        if (!images_s[i]) {
            printf("IMG_Load: %s\n", IMG_GetError());
            return 1;
        }
        textures_s[i] = SDL_CreateTextureFromSurface(renderer, images_s[i]);
        if (!textures_s[i]) {
            printf("SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
            return 1;
        }
        SDL_FreeSurface(images_s[i]);
    }


    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderClear(renderer);

    int stage = 1;
    if (stage == 1) {
		Renderinit1(renderer, textures);
	}
	else if (stage == 2 || stage == 3) {
		Renderinit23(renderer, textures);
	}
    else if (stage == 4 || stage == 5) {
        Renderinit45(renderer, textures);
    }
    else if (stage == 6) {
        Renderinit6(renderer, textures);
    }

    int selected_index[2] = { -1, -1 };
    int prev_selected_index[2] = { -1, -1 };
    SDL_Color textColor = { 0, 0, 0, 255 }; // 黒色

    Uint32 startTime = SDL_GetTicks();
    Uint32 lastUpdateTime = startTime;
    int bonusTime = 0;
    SDL_Surface* textSurface = NULL;
    // event handling
    SDL_Event e;
    int prev_elapsedTime = -1;
    SDL_Rect textRect = { 635, 100, 160, 20 }; // テキストの描画領域
    Uint32 elapsedTime = 155;
    bool timeUpWindowShown = false;
    bool timeflag = true;
    int count = 144;
    bool clearWindowShown = false;
    bool stop = false;
    Uint32 pauseTime = 0;
    Uint32 stopTime = 0;
    int pair[2][2] = { {0,0},{0,0} };
    bool hint = false;
	int temp_selected_index[2] = { -1, -1 };

    findPair(imagearray, pair, renderer, textures);
    renderStage(renderer, font, stage);

    // 描画ループ
    while (true) {
        
        SDL_RenderPresent(renderer);
        // ボタンの描画
        renderButton(button1, renderer);
        renderButton(button2, renderer);
        

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastUpdateTime >= 1000 || currentTime < 1000) {
            lastUpdateTime = currentTime;
            if (stop) {
                pauseTime += (currentTime - stopTime);
                stopTime = currentTime;
            }
            elapsedTime = 155 - (currentTime - pauseTime - startTime) / 1000 + bonusTime;
            if (elapsedTime > 1000000) {
                elapsedTime = 0;
            }
            timeflag = true;
            std::string timeText = "Time: " + std::to_string(elapsedTime) + " seconds";

            textSurface = TTF_RenderText_Solid(font, timeText.c_str(), textColor);
            if (textSurface == nullptr) {
                std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
            }
            else if(prev_elapsedTime != elapsedTime){
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                if (textTexture == nullptr) {
                    std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
                }
                else {
                    // テキストの描画領域をクリア
                    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // 黒色でクリア
                    SDL_RenderFillRect(renderer, &textRect);
                    SDL_Rect renderQuad = { 635, 100, 160, 20 };
                    
                    SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
                    SDL_RenderPresent(renderer);
                    SDL_DestroyTexture(textTexture);
                    prev_elapsedTime = elapsedTime;
                }
                
            }
        }
        bool a = false;
        // elapsedTimeが0になったら新しいウィンドウを表示
        if (elapsedTime == 0 && !timeUpWindowShown && timeflag) {
            a = subWindow(0);
            timeUpWindowShown = true;
            selected_index[0] = -1;
            selected_index[1] = -1;
        }
        if (a) {
            timeUpWindowShown = false;
            a = false;
            startTime = SDL_GetTicks();
            currentTime = SDL_GetTicks();
            timeflag = false;
            stage = 1;
            Renderinit1(renderer, textures);
            bonusTime = 0;
            pauseTime = 0;
            count = 144;
            SDL_Rect textRect2 = { 635, 400, 160, 50 }; // テキスト領域
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // 囲む色
            SDL_RenderFillRect(renderer, &textRect2);
            renderStage(renderer, font, stage);
            findPair(imagearray, pair, renderer, textures);
        }

        bool b = false;
        // countが0になったら新しいウィンドウを表示
        if (count == 0 && !clearWindowShown && timeflag) {
            b = subWindow(1);
            clearWindowShown = true;
            selected_index[0] = -1;
            selected_index[1] = -1;
            if (stage != 6) {
                stage++;
            }
            else {
				stage = 1;
            }
        }
        if (b) {
            clearWindowShown = false;
            b = false;
            timeflag = false;
            startTime = SDL_GetTicks();
            currentTime = SDL_GetTicks();
			if (stage == 1) {
				Renderinit1(renderer, textures);
			}
			else if (stage == 2 || stage == 3) {
				Renderinit23(renderer, textures);
			}
            else if (stage == 4 || stage == 5) {
                Renderinit45(renderer, textures);
            }
            else if (stage == 6) {
                Renderinit6(renderer, textures);
            }
            count = 144;
            bonusTime = 0;
            pauseTime = 0;
            SDL_Rect textRect2 = { 635, 400, 160, 50 }; // テキスト領域
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // 囲む色
            SDL_RenderFillRect(renderer, &textRect2);
            renderStage(renderer, font, stage);
            findPair(imagearray, pair, renderer, textures);
        }
        
        
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                break;
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                break;
            }
            else if (handleMouseEvent(button1, &e)) {
                if (!stop) {
                    SDL_Rect Image = { 30, 30, 600, 600 };
                    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // 背景色を設定
                    SDL_RenderFillRect(renderer, &Image);
                    stopTime = SDL_GetTicks();
                    stop = true;
                }
                else {
                    restart(renderer, textures);
                    stop = false;
                    selected_index[0] = -1;
                    selected_index[1] = -1;
                }
            }
            else if (handleMouseEvent(button2, &e)) {
                int index1 = imagearray[pair[0][1]+1][pair[0][0]+1];
                int index2 = imagearray[pair[1][1]+1][pair[1][0]+1];
				int prev_index = imagearray[selected_index[1] + 1][selected_index[0] + 1];
                hint = true;
                bonusTime -= 6;
                
                SDL_Rect dstRect1 = { pair[0][0] * IMAGE_WIDTH + IMAGE_INIT, pair[0][1] * IMAGE_HEIGHT + IMAGE_INIT , IMAGE_WIDTH - 2, IMAGE_HEIGHT - 2 };
                SDL_RenderCopy(renderer, textures_s[index1 - 1], NULL, &dstRect1);
                SDL_Rect dstRect2 = { pair[1][0] * IMAGE_WIDTH + IMAGE_INIT, pair[1][1] * IMAGE_HEIGHT + IMAGE_INIT , IMAGE_WIDTH - 2, IMAGE_HEIGHT - 2 };
                SDL_RenderCopy(renderer, textures_s[index2 - 1], NULL, &dstRect2);
				if (selected_index[0] != -1) {
					SDL_Rect dstRect = { selected_index[0] * IMAGE_WIDTH + IMAGE_INIT, selected_index[1] * IMAGE_HEIGHT + IMAGE_INIT , IMAGE_WIDTH - 2, IMAGE_HEIGHT - 2 };
					SDL_RenderCopy(renderer, textures[prev_index - 1], NULL, &dstRect);
				}

                selected_index[0] = -1;
                selected_index[1] = -1;

            }
            else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
                if (mouse_position.x <= IMAGE_INIT || mouse_position.y <= IMAGE_INIT ||
                    mouse_position.x >= IMAGE_WIDTH * 12 + IMAGE_INIT || mouse_position.y >= IMAGE_HEIGHT * 12 + IMAGE_INIT) {
                    continue;
                }
                if (hint) {
                    int index1 = imagearray[pair[0][1] + 1][pair[0][0] + 1];
                    int index2 = imagearray[pair[1][1] + 1][pair[1][0] + 1];
                    SDL_Rect dstRect1 = { pair[0][0] * IMAGE_WIDTH + IMAGE_INIT, pair[0][1] * IMAGE_HEIGHT + IMAGE_INIT , IMAGE_WIDTH - 2, IMAGE_HEIGHT - 2 };
                    SDL_RenderCopy(renderer, textures[index1 - 1], NULL, &dstRect1);
                    SDL_Rect dstRect2 = { pair[1][0] * IMAGE_WIDTH + IMAGE_INIT, pair[1][1] * IMAGE_HEIGHT + IMAGE_INIT , IMAGE_WIDTH - 2, IMAGE_HEIGHT - 2 };
                    SDL_RenderCopy(renderer, textures[index2 - 1], NULL, &dstRect2);
                    hint = false;
                }
                prev_selected_index[0] = selected_index[0];
                prev_selected_index[1] = selected_index[1];
                int prev_index = imagearray[prev_selected_index[1] + 1][prev_selected_index[0] + 1];
                int row = (mouse_position.y - IMAGE_INIT) / IMAGE_HEIGHT;
                int col = (mouse_position.x - IMAGE_INIT) / IMAGE_WIDTH;
                int index = imagearray[row + 1][col + 1];
                int flag = 0;
                selected_index[0] = col;
                selected_index[1] = row;
                if (index == 0) {
                    selected_index[0] = prev_selected_index[0];
                    selected_index[1] = prev_selected_index[1];
                    continue;
                }
                if (prev_selected_index[0] == -1) {
                    SDL_Rect dstRect = { col * IMAGE_WIDTH + IMAGE_INIT, row * IMAGE_HEIGHT + IMAGE_INIT , IMAGE_WIDTH - 2, IMAGE_HEIGHT - 2 };
                    SDL_RenderCopy(renderer, textures_s[index - 1], NULL, &dstRect);
                    continue;
                }
                SDL_Rect prev_dstRect = { prev_selected_index[0] * IMAGE_WIDTH + IMAGE_INIT, prev_selected_index[1] * IMAGE_HEIGHT + IMAGE_INIT , IMAGE_WIDTH - 2, IMAGE_HEIGHT - 2 };
                SDL_RenderCopy(renderer, textures[prev_index - 1], NULL, &prev_dstRect);
                SDL_Rect dstRect = { selected_index[0] * IMAGE_WIDTH + IMAGE_INIT, selected_index[1] * IMAGE_HEIGHT + IMAGE_INIT , IMAGE_WIDTH - 2, IMAGE_HEIGHT - 2 };
                SDL_RenderCopy(renderer, textures_s[index - 1], NULL, &dstRect);
                if (prev_index == index && (prev_selected_index[0] != selected_index[0] || prev_selected_index[1] != selected_index[1])) {


                    int prev_row = prev_selected_index[1];
                    int prev_col = prev_selected_index[0];

                    flag = checkArrays(prev_row, prev_col, row, col, imagearray);
                    

                    if (flag == 1) {
						temp_selected_index[0] = selected_index[0];
						temp_selected_index[1] = selected_index[1];
                        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // 背景色を設定
                        SDL_RenderFillRect(renderer, &prev_dstRect);
                        SDL_RenderFillRect(renderer, &dstRect);
                        imagearray[prev_selected_index[1] + 1][prev_selected_index[0] + 1] = 0;
                        imagearray[selected_index[1] + 1][selected_index[0] + 1] = 0;
                        selected_index[0] = -1;
                        selected_index[1] = -1;
                        flag = 0;
                        bonusTime += 3;
                        count -= 2;
                        if (count == 0) {
                            continue;
                        }
                        if (stage == 3 || stage == 5) {
                            removeAndShift(imagearray, temp_selected_index, prev_selected_index);
                            
                            SDL_Rect Image = { 30, 30, 600, 600 };
                            SDL_RenderFillRect(renderer, &Image);
							restart(renderer, textures);
                        }

                        findPair(imagearray, pair, renderer, textures);
                    }
                    
                }
            }
        }
        SDL_Delay(1);

    }
        // クリーンアップ
        for (int i = 0; i < IMAGE_NUM; ++i) {
            SDL_DestroyTexture(textures[i]);
        }
        close(window, renderer, NULL, font);
        SDL_FreeSurface(textSurface);

        return 0;
    
}

bool init(SDL_Window** window, SDL_Renderer** renderer, TTF_Font** font) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    *window = SDL_CreateWindow("Link", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == nullptr) {
        SDL_DestroyRenderer(*renderer);
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return 1;
    }

    *font = TTF_OpenFont("fonts/arial.ttf", 24);
    if (font == nullptr) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawColor(*renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    return true;
}

void close(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, TTF_Font* font) {
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
    }
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}


