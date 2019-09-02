#include "sound.h"

sf::Int16 max(const sf::Int16* array, sf::Uint64 arraySize)
{
    sf::Int16 max = 0;
    for (sf::Uint64 i = 0; i < arraySize; ++i)
        if(array[i] > max)
            max = array[i];
    return max;
}

double average(const fftw_complex* array, sf::Uint64 arraySize)
{
    double average = 0;
    for (sf::Uint64 i = 0; i < arraySize; ++i)
        average += norm(array[i]);
    return average/arraySize;
}

double max(const fftw_complex* array, sf::Uint64 arraySize)
{
    double max = 0;
    for (sf::Uint64 i = 0; i < arraySize; ++i)
        if(norm(array[i]) > max)
            max = norm(array[i]);
    return max;
}

namespace ad
{
    Sound::Sound(std::string textureName, std::string soundOutputFileName)
    {
        this->soundOutputFileName = soundOutputFileName;
        this->texture.loadFromFile(textureName);
        this->sprite.setTexture(texture);
        this->sprite.setPosition(0.0, 0.0);
        this->k = 1.0;
        this->duration = 5.0;
        this->isRecording = false;
        this->isScalable = false;
        this->isPlayable = false;
    }

    void Sound::record(sf::RenderWindow& window)
    {
        if(isRecording)
        {
            window.draw(sprite);
            if(clock.getElapsedTime().asSeconds() > duration)
            {
                recorder.stop();
                isRecording = false;
                isScalable = true;
                buffer = recorder.getBuffer();
                sound.setBuffer(buffer);
                isPlayable = true;
            }
        }
    }

    void Sound::record()
    {
        if(!isRecording)
        {
            recorder.start();
            clock.restart();
            isRecording = true;
        }
    }

    void Sound::scale()
    {

        if(isScalable)
        {
                const sf::Int16* recorderBuffer = recorder.getBuffer().getSamples();
                IntermidiateSoundBufferSamples = new sf::Int16[recorder.getBuffer().getSampleCount()];
                for (sf::Uint64 i = 0; i < buffer.getSampleCount();++i)
                    IntermidiateSoundBufferSamples[i] = recorderBuffer[i];

                sf::Uint64 N = recorder.getBuffer().getSampleCount();
                fftw_complex *soundSpectrum = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
                for(sf::Uint64 i = 0; i < N; ++i)
                {
                    soundSpectrum[i][0] = double(IntermidiateSoundBufferSamples[i]);
                    soundSpectrum[i][1] = 0;
                }
                fftw_plan transform = fftw_plan_dft_1d(int(N), soundSpectrum, soundSpectrum, FFTW_FORWARD, FFTW_ESTIMATE);
                fftw_execute(transform);
                fftw_destroy_plan(transform);

                double average_ = average(soundSpectrum, N);
                for(sf::Uint64 i = 0; i < N; ++i)
                {
                    if(norm(soundSpectrum[i]) < average_)
                    {
                        soundSpectrum[i][0] = 0.0;
                        soundSpectrum[i][1] = 0.0;
                    }
                }

                fftw_complex *newSoundSpectrum = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
                for(sf::Uint64 i = 1; i < N / 2; ++i)
                {
                    newSoundSpectrum[i][0] = soundSpectrum[sf::Uint64(floorf(i / k))][0];
                    newSoundSpectrum[i][1] = soundSpectrum[sf::Uint64(floorf(i / k))][1];
                }
                for(sf::Uint64 i = 0; i < N / 2; ++i)
                {
                    newSoundSpectrum[N - i][0] = soundSpectrum[N - sf::Uint64(floorf(i / k))][0];
                    newSoundSpectrum[N - i][1] = soundSpectrum[N - sf::Uint64(floorf(i / k))][1];
                }
                //------------------------------------------------------------------------------------------------------
                std::ofstream spectrumFile, newSpectrumFile;
                spectrumFile.open("sfile.txt");
                newSpectrumFile.open("nsfile.txt");
                for (sf::Uint64 i = 0; i < N; ++i)
                {
                    spectrumFile << norm(soundSpectrum[i]) << std::endl;
                    newSpectrumFile << norm(newSoundSpectrum[i]) << std::endl;
                }
                //------------------------------------------------------------------------------------------------------
                transform = fftw_plan_dft_1d(N, newSoundSpectrum, newSoundSpectrum, FFTW_BACKWARD ,FFTW_ESTIMATE);
                fftw_execute(transform);
                fftw_destroy_plan(transform);

                sf::Uint64 size = k < 1.0f ? N : sf::Uint64(floorf(N/k));
                double scaleFactor = 0.1 * max(recorder.getBuffer().getSamples(), recorder.getBuffer().getSampleCount())/max(newSoundSpectrum, size);
                for(sf::Uint64 i = 0; i < size; ++i)
                {
                    IntermidiateSoundBufferSamples[i] = sf::Int16(norm(newSoundSpectrum[i]) * 1e-7);
                    //std::cout << IntermidiateSoundBufferSamples[i] << std::endl;
                }

                this->buffer.loadFromSamples(IntermidiateSoundBufferSamples, size, recorder.getBuffer().getChannelCount(), recorder.getBuffer().getSampleRate());
                this->sound.setBuffer(this->buffer);
                this->sound.play();
                fftw_free(soundSpectrum);
                fftw_free(newSoundSpectrum);
                isPlayable = true;
            }
    }

    void Sound::setDuration(float duration)
    {
        this->duration = duration;
    }

    void Sound::setScale(float k)
    {
        this->k = k;
    }

    void Sound::play()
    {
        if(isPlayable)
        {
            sound.play();
            isPlayable = false;
        }
    }
}


