#ifndef STUDY_H
#define STUDY_H

#include "sierrachart.h"

enum StudyType
{
    STUDY_COMBO,
    STUDY_SLIDER,
    STUDY_THEME,
    STUDY_POWERMETER
};

class Study
{
public:
    Study(SCStudyInterfaceRef sc);
    void Run();
    static Study* StartStudy(StudyType StudyType_, SCStudyInterfaceRef sc_);

protected:
    SCStudyInterfaceRef _sc;
    bool Initialised;

    virtual void Init() = 0;
    virtual void DoSetDefaults() = 0;
    virtual void DoStudy() = 0;
    virtual void CleanUp() = 0;
    void SetReferences(SCStudyInterfaceRef* sc);
};

#endif