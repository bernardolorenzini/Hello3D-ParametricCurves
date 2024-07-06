#include "Curve.h"
class CatmullRom :
    public Curve
{
public:
    CatmullRom();
    void generateCurve(int pontos_de_segmentos);
};

