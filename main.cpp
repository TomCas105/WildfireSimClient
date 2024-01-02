#include <iostream>
#include <vector>

using namespace std;

struct TileTransformation {
    float _transformChance;
    int _transformTile;
    int _requiredTile;
};

class TileType {
private:
    int _type;
    bool _onFire;
    float _fireWind;
    float _fireNoWind;
    float _fireAgainstWind;
    vector<TileTransformation> _transforms;
public:
    TileType();

    ~TileType();

    static TileType getBurntTileType() {
        TileType tileType;
        tileType._type = 0;
        tileType._transforms.push_back(TileTransformation{
                ._transformChance = 0.1f,
                ._transformTile = 2,
                ._requiredTile = 4
        });
        return tileType;
    }

    static TileType getForestTileType() {
        TileType tileType;
        tileType._type = 1;
        tileType._fireWind = 0.9f;
        tileType._fireNoWind = 0.2f;
        tileType._fireAgainstWind = 0.02f;
        return tileType;
    }

    static TileType getGrassTileType() {
        TileType tileType;
        tileType._type = 2;
        tileType._fireWind = 0.9f;
        tileType._fireNoWind = 0.2f;
        tileType._fireAgainstWind = 0.02f;
        tileType._transforms.push_back(TileTransformation{
                ._transformChance = 0.02f,
                ._transformTile = 1,
                ._requiredTile = 1
        });
        return tileType;
    }

    static TileType getRocksTileType() {
    }

    static TileType getWaterTileType() {
    }
};

TileType::TileType() {
    _type = 1;
    _onFire = false;
}

//0 zhorena, les 1, luka 2, skaly 3, voda 4

int main() {
    //Tile type init
    TileType tileTypes[5];
    tileTypes[0] = TileType::getBurntTileType();
    tileTypes[1] = TileType::getForestTileType();
    tileTypes[2] = TileType::getGrassTileType();
    tileTypes[3] = TileType::getRocksTileType();
    tileTypes[4] = TileType::getWaterTileType();

    int tiles[50][50];

    return 0;
}
