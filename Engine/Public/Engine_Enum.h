#pragma once

namespace Engine
{
    enum MOUSEMOVESTATE { MMS_X, MMS_Y, MMS_WHEEL, MMS_END };


    enum ROOTBONETRANSFORMTYPE { SCALE, ROTATION, TRANSLATION };

    enum PASSID_PARTICLE { PARTICLE_DEFAULT, PARTICLE_ICICLE, PARTICLE_FEATHER, PARTICLE_FEATHERA, PARTICLE_SHIELDBREAK,
        PARTICLE_SHARD, PARTICLE_BRIGHT_SHARD, PARTICLE_SOLID_DEBRIS, PARTICLE_FIRE, PARTICLE_TORNADO, PARTICLE_DUST, PARTICLE_HEAT_SMOKE, 
        PARTICLE_BLINK_SHRINK, PARTICLE_STAR_COLOR, PARTICLE_TRAILING_LINE, PARTICLE_WATER_FOG, PARTICLE_EFFIGY_PAPERBURN, 
        PARTICLE_EFFIGY_SPARK, PARTICLE_PHOENIX_SMOKE, PARTICLE_DVL_STARRAIN,
        PARTICLE_END };
    enum PASSID_TRAIL { TRAIL_DEFAULT, TRAIL_OVER, TRAIL_ANDRIUS, TRAIL_DILUC, TRAIL_ELEMENT, TRAIL_EFFIGYFIRE, TRAIL_END };
    enum PASSID_DEFERRED    { DEFERRED_SINGLEDEBUG, DEFERRED_MULTIDEBUG,
        DEFERRED_DIRLIGHT, DEFERRED_POINTLIGHT,
        DEFERRED_SCENE, DEFERRED_EFFECT, DEFERRED_END };

    enum CHANNELID {
        SOUND_BGM, SOUND_VOICE, SOUND_ANNOUNCE,
        SOUND_CH0,  SOUND_CH1,  SOUND_CH2,  SOUND_CH3,
        SOUND_CH4,  SOUND_CH5,  SOUND_CH6,  SOUND_CH7,
        SOUND_CH8,  SOUND_CH9,  SOUND_CH10, SOUND_CH11,
        SOUND_CH12, SOUND_CH13, SOUND_CH14, SOUND_CH15,
        SOUND_CH16, SOUND_CH17, SOUND_CH18, SOUND_CH19,
        SOUND_CH20, SOUND_CH21, SOUND_CH22, SOUND_CH23,
        SOUND_CH24, SOUND_CH25, SOUND_CH26, SOUND_CH27,
        SOUND_CHMAX,
        MAXCHANNEL
    };

    enum TEXTURETYPE
    {
        aiTextureType_NONE = 0,
        aiTextureType_DIFFUSE = 1,
        aiTextureType_SPECULAR = 2,
        aiTextureType_AMBIENT = 3,
        aiTextureType_EMISSIVE = 4,
        aiTextureType_HEIGHT = 5,
        aiTextureType_NORMALS = 6,
        aiTextureType_SHININESS = 7,
        aiTextureType_OPACITY = 8,
        aiTextureType_DISPLACEMENT = 9,
        aiTextureType_LIGHTMAP = 10,
        aiTextureType_REFLECTION = 11,
        aiTextureType_BASE_COLOR = 12,
        aiTextureType_NORMAL_CAMERA = 13,
        aiTextureType_EMISSION_COLOR = 14,
        aiTextureType_METALNESS = 15,
        aiTextureType_DIFFUSE_ROUGHNESS = 16,
        aiTextureType_AMBIENT_OCCLUSION = 17,
        aiTextureType_UNKNOWN = 18,
#ifndef SWIG
        _aiTextureType_Force32Bit = INT_MAX
#endif
    };

    union CellOffset
    {
        struct
        {
            int16_t CellX;
            int16_t CellY;
            int16_t CellZ;
        };

        int64_t Checksum;

        bool operator>(const CellOffset& _Target) const
        {
            return Checksum > _Target.Checksum;
        }
        bool operator<(const CellOffset& _Target) const
        {
            return Checksum < _Target.Checksum;
        }
        bool operator==(const CellOffset& _Target) const
        {
            return Checksum == _Target.Checksum;
        }
    };
}