#include "component.h"

int main()
{
    CanDevice canDev;
    CanFilter canFilter;
    CanRawView canView;

    canDev.connect(CanFrameSource, &canFilter);
    canFilter.connect(CanFrameSource, &canView);

    return 0;
}
