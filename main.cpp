#include "sound.h"

int main()
{
    ad::Sound sound("texture.png", "file.ogg");
    sf::RenderWindow window(sf::VideoMode(1000, 500), "SFML window");

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color(64, 64, 64));

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            sound.setDuration(7.0);
            sound.record();
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            //sound.setScale(2.0);
            //sound.scale();
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::P))
        {
            sound.play();
        }

        sound.record(window);
        window.display();
    }
    return EXIT_SUCCESS;
}
