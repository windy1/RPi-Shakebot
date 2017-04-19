#include "Graphics.h"
#include "../app.h"
#include <iostream>

namespace sb {

    const string Graphics::WINDOW_TITLE("Shakebot 1.0");
    const sf::VideoMode Graphics::VIDEO_MODE(440 * 2, 564 * 2);

    void Graphics::init() {
        sbRender = new sb::RenderShakebot(sb::getBot(), "images/shakespeare.jpg", sf::IntRect(166, 243, 54, 65));
        window.create(VIDEO_MODE, WINDOW_TITLE);
        window.setKeyRepeatEnabled(false);
    }

    void Graphics::clear() {
        window.clear(sf::Color::Black);
    }

    static bool recording = false;

    void Graphics::pollInput() {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::Num1:
                            cout << ">> ";
                            getline(cin, nextPhrase);
                            cout << nextPhrase << endl;
                            clock.restart();
                            break;
                        case sf::Keyboard::Return:
                            if (!recording) {
                                cout << "recording" << endl;
                                recording = true;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::KeyReleased:
                    switch (event.key.code) {
                        case sf::Keyboard::Return:
                            if (recording) {
                                cout << "done recording" << endl;
                                recording = false;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
    }

    void Graphics::push() {
        if (!nextPhrase.empty() && clock.getElapsedTime() >= sf::seconds(3)) {
            sb::getBot()->say(nextPhrase);
            nextPhrase.clear();
        }
        sbRender->draw(&window);
        window.display();
        sb::setRunning(window.isOpen());
    }

}
