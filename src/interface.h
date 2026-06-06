#ifndef SRC_INTERFACE
#define SRC_INTERFACE

#include "renderer.h"
#include <string>

class Interface : public Renderer
{
public:
    Interface(SDL_Renderer *renderer);
    ~Interface();
    bool render(AVFrame *frame);
    bool drawHome(bool force, int state, std::string name);
    void debug(const char *text);
    void showToast(const std::string &text);
    void hideToast();

private:
    void drawDebug();
    void drawToast();
    void drawExitButton();

    int _state;
    bool _debug;
    bool _toast;
    RendererText _textStatus;
    RendererText _textDebug;
    RendererText _textToast;
    RendererImage _mainImage;
    std::string _debugText;
    std::string _toastText;
    RendererText _textExit;
    static constexpr int EXIT_BTN = 60;
};

#endif /* SRC_INTERFACE */
