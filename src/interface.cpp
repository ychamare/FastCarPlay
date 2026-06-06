#include "interface.h"
#include "resource/background.h"
#include "resource/font.h"
#include "resource/colours.h"
#include "settings.h"
#include "protocol/protocol_const.h"

Interface::Interface(SDL_Renderer *renderer)
    : Renderer(renderer),
      _state(0),
      _debug(false),
      _toast(false),
      _textStatus(font, font_len, Settings::fontSize),
      _textDebug(font, font_len, 16),
      _textToast(font, font_len, Settings::fontSize*0.75),
      _textExit(font, font_len, 28),
      _mainImage(background, background_len)
{
}

Interface::~Interface()
{
}

bool Interface::render(AVFrame *frame)
{
    if (!frame)
        return false;

    if (_render == nullptr || frame->width != _textureWidth || frame->height != _textureHeight)
    {
        clear();
        if (!prepare(frame, Settings::width, Settings::height))
            return false;
    }

    (this->*_render)(frame);
    SDL_RenderCopy(_renderer, _texture, &_sourceRect, nullptr);
    drawExitButton();

    if (_toast)
        drawToast();

#ifndef NDEBUG
    if (_debug)
    {
        drawDebug();
        _debug = false;
    }
#endif

    SDL_RenderPresent(_renderer);
    return true;
}

bool Interface::drawHome(bool force, int state, std::string name)
{
    if (state == _state && !force)
        return false;

    _state = state;
    int width, height;
    SDL_GetRendererOutputSize(_renderer, &width, &height);

    _mainImage.draw(_renderer, width, height);
    bool drawText = false;

    if (state == PROTOCOL_STATUS_ERROR)
        if (_textStatus.prepare(_renderer, "Dongle error", colorError))
            drawText = true;

    if (state == PROTOCOL_STATUS_NO_DEVICE)
        if (_textStatus.prepare(_renderer, "Insert dongle", colorError))
            drawText = true;

    if (state == PROTOCOL_STATUS_INITIALISING)
        if (_textStatus.prepare(_renderer, "Initialising", color2))
            drawText = true;

    if (state == PROTOCOL_STATUS_LINKING)
        if (_textStatus.prepare(_renderer, "Initialising", color2))
            drawText = true;

    if (state == PROTOCOL_STATUS_ONLINE)
        if (_textStatus.prepare(_renderer, "Connect phone", color4))
            drawText = true;

    if (state == PROTOCOL_STATUS_CONNECTED)
    {
        if(name.length()>0)
            name = " to "+name;
        if (_textStatus.prepare(_renderer, "Connecting"+name, color3))
            drawText = true;
    }

    if (drawText)
        _textStatus.draw(_renderer, (width - _textStatus.width * Settings::aspectCorrection) / 2, height * 0.85 - _textStatus.height);

    if (_toast)
        drawToast();

    if (_debug)
    {
        drawDebug();
        _debug = false;
    }

    drawExitButton();
    SDL_RenderPresent(_renderer);
    return true;
}

void Interface::debug(const char *text)
{
    _debugText = text ? text : "";
    _debug = true;
}

void Interface::showToast(const std::string &text)
{
    _toastText = text;
    _toast = true;
}

void Interface::hideToast()
{
    _toastText.clear();
    _toast = false;
}

void Interface::drawDebug()
{
    if (_debugText.empty())
        return;

    constexpr int padding = 8;
    constexpr int lineSpacing = 2;
    const SDL_Color debugColor = {255, 255, 255, 255};
    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 150);

    size_t lineStart = 0;
    int y = padding;

    while (lineStart <= _debugText.size())
    {
        size_t lineEnd = _debugText.find('\n', lineStart);
        std::string line = _debugText.substr(lineStart, lineEnd - lineStart);
        if (_textDebug.prepare(_renderer, line, debugColor))
        {
            SDL_Rect backgroundRect = {
                0,
                y,
                static_cast<int>(_textDebug.width * Settings::aspectCorrection) + padding * 2,
                _textDebug.height};
            SDL_RenderFillRect(_renderer, &backgroundRect);
            _textDebug.draw(_renderer, padding, y);
        }
        y += _textDebug.height + lineSpacing;

        if (lineEnd == std::string::npos)
            break;

        lineStart = lineEnd + 1;
    }
}

void Interface::drawToast()
{
    if (_toastText.empty())
        return;

    int padding = Settings::fontSize*0.3;
    int width, height;
    SDL_GetRendererOutputSize(_renderer, &width, &height);
    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 150);

    if (_textToast.prepare(_renderer, _toastText, color4))
    {
        SDL_Rect backgroundRect = {0, 0, width, _textToast.height + padding * 2};
        SDL_RenderFillRect(_renderer, &backgroundRect);
        _textToast.draw(_renderer, (width - _textToast.width * Settings::aspectCorrection) / 2, padding);
    }
}

void Interface::drawExitButton()
{
    int w, h;
    SDL_GetRendererOutputSize(_renderer, &w, &h);

    // Position: top-right corner with 8px margin
    SDL_Rect btn = {w - EXIT_BTN - 8, 8, EXIT_BTN, EXIT_BTN};

    // Semi-transparent dark background
    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_renderer, 20, 20, 20, 180);
    SDL_RenderFillRect(_renderer, &btn);

    // Subtle border
    SDL_SetRenderDrawColor(_renderer, 180, 180, 180, 160);
    SDL_RenderDrawRect(_renderer, &btn);

    // Centre the X text in the button
    SDL_Color white = {255, 255, 255, 230};
    if (_textExit.prepare(_renderer, "X", white))
    {
        int tx = btn.x + (EXIT_BTN - _textExit.width) / 2;
        int ty = btn.y + (EXIT_BTN - _textExit.height) / 2;
        _textExit.draw(_renderer, tx, ty);
    }
}
