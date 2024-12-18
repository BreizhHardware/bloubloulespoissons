#ifndef MUSIC_H
#define MUSIC_H

#include <SDL2/SDL_mixer.h>
#include <string>
#include <iostream>

enum class AudioType {
    MUSIC,
    SOUND_EFFECT
};

enum AudioChannel {
    MUSIC_CHANNEL = 0,
    SOUND_CHANNEL = 1
};

struct Audio {
    Mix_Music* music;
    Mix_Chunk* sound;
};

class Music {
    private:
        AudioType type;
        std::vector<Audio> audio;
        std::string path;
        std::string name;
        AudioChannel channel;
        
    public:
        Music(std::string filePath, AudioType audio_type){
            name = getNameFromPath(filePath);
            path = filePath;
            type = audio_type;
            std::cout << "Loading " << name << " from " << path << std::endl;
            if(type == AudioType::MUSIC){
                channel = MUSIC_CHANNEL;
                Mix_Music* music = Mix_LoadMUS(filePath.c_str());
                Audio sss;
                sss.music = music;
                if(music == nullptr){
                    std::cerr << "Error loading music: " << Mix_GetError() << std::endl;
                }else{
                    audio.push_back(sss);
                }
            }else if (type == AudioType::SOUND_EFFECT)
            {
                channel = SOUND_CHANNEL;
                Mix_Chunk* sound = Mix_LoadWAV(filePath.c_str());
                Audio sss;
                sss.sound = sound;
                if(sound == nullptr){
                    std::cerr << "Error loading sound effect: " << Mix_GetError() << std::endl;
                }else{
                    audio.push_back(sss);
                }
            }
        }

        // ~Music(){
        //     if(type == AudioType::MUSIC){
        //         Mix_FreeMusic(audio[0].music);
        //     }else if (type == AudioType::SOUND_EFFECT)
        //     {
        //         Mix_FreeChunk(audio[0].sound);
        //     }
        // }

        std::string getName(){
            return name;
        }

        std::string getNameFromPath(std::string path){
            std::string name = path.substr(path.find_last_of("/\\") + 1);
            name = name.substr(0, name.find_last_of("."));
            std::cout << "Name: " << name << std::endl;
            return name;
        }

        void play(){
            if(type == AudioType::MUSIC){
                Mix_HaltChannel(channel);
                Mix_PlayMusic(audio[0].music, channel);
            }else if (type == AudioType::SOUND_EFFECT)
            {
                Mix_HaltChannel(channel);
                Mix_PlayChannel(channel, audio[0].sound, 0);
            }
        }

        void stop(){
            if(type == AudioType::MUSIC){
                Mix_HaltChannel(channel);
            }else if (type == AudioType::SOUND_EFFECT)
            {
                Mix_HaltChannel(channel);
            }
        }

        void pause(){
            if(type == AudioType::MUSIC){
                Mix_PauseMusic();
            }else if (type == AudioType::SOUND_EFFECT)
            {
                Mix_Pause(channel);
            }
        }

        void resume(){
            if(type == AudioType::MUSIC){
                Mix_ResumeMusic();
            }else if (type == AudioType::SOUND_EFFECT)
            {
                Mix_Resume(channel);
            }
        }

};



class MusicManager {
    private: 
        std::vector<Music> musicList;
        int volume = MIX_MAX_VOLUME;
    public:

        MusicManager(){
            if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
                std::cerr << "Error initializing SDL_mixer: " << Mix_GetError() << std::endl;
            }
            Mix_Volume(-1, volume);
            Mix_VolumeMusic(volume);
        }

        ~MusicManager(){
            for(Music music : musicList){
                music.stop();
            }
            Mix_CloseAudio();
        }

        void addMusic(std::string filePath, AudioType audio_type){
            Music music(filePath, audio_type);
            musicList.push_back(music);
        }

        void playMusic(std::string name){
            for(Music music : musicList){
                if(music.getName() == name){
                    music.play();
                    std::cout << "Started playing " << name << std::endl;
                }
            }
        }

        void stopMusic(std::string name){
            for(Music music : musicList){
                if(music.getName() == name){
                    music.stop();
                }
            }
        }

        void resumeMusic(std::string name){
            for(Music music : musicList){
                if(music.getName() == name){
                    music.resume();
                }
            }
        }

        void setVolume(int volume){
            this->volume = volume;
            Mix_Volume(-1, volume);
            Mix_VolumeMusic(volume);
        }

        void pauseMusic(std::string name){
            for(Music music : musicList){
                if(music.getName() == name){
                    music.pause();
                }
            }
        }


    
};

#endif