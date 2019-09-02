#ifndef SOUND_H
#define SOUND_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <fftw3.h>

#define norm(x) (sqrt(x[0]*x[0]+x[1]*x[1]))

namespace ad
{
    class Sound
    {
    public:
        Sound(std::string textureName, std::string soundOutputFileName);
        void record(sf::RenderWindow& window);
        void record();
        void setScale(float k);
        void setDuration(float duration);
        void filter(float filterAmp);
        void scale();
        void play();
    private:
        bool isRecording;
        bool isScalable;
        bool isPlayable;

        float k;
        float duration;
        sf::Clock clock;

        sf::Texture texture;
        sf::Sprite sprite;

        sf::SoundBufferRecorder recorder;
        sf::SoundBuffer buffer;
        sf::Sound sound;
        sf::Int16* IntermidiateSoundBufferSamples;

        std::string soundOutputFileName;

        sf::RenderWindow window;
    };
}

#endif // SOUND_H
