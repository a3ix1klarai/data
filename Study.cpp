#include "Study.h"
#include "OrderManager2107.h"

Study::Study(SCStudyInterfaceRef sc) : _sc(sc), Initialised(false) {}

void Study::Run()
{
    if (!Initialised)
    {
        Init();
    }

    if (_sc.SetDefaults)
    {
        DoSetDefaults();
        return;
    }

    if (_sc.LastCallToFunction)
    {
        CleanUp();
        return;
    }

    DoStudy();
}

Study* Study::StartStudy(StudyType StudyType_, SCStudyInterfaceRef sc_)
{
    const bool IsDLLInit = sc_.SetDefaults && sc_.ArraySize == 0;

    Study* study = IsDLLInit ? NULL : static_cast<Study*>(sc_.GetPersistentPointer(1));

    if (study == NULL)
    {
        switch (StudyType_)
        {
        case STUDY_COMBO:
            study = new ComboStudy(sc_);
            break;
        case STUDY_SLIDER:
            study = new SliderStudy(sc_);
            break;
        case STUDY_THEME:
            study = new ThemeStudy(sc_);
            break;
        case STUDY_POWERMETER:
            study = new PowerMeterStudy(sc_);
            break;
        }
        sc_.SetPersistentPointer(1, study);
    }

    study->SetReferences(&sc_);

    study->Run();

    if (sc_.LastCallToFunction || IsDLLInit)
    {
        delete study;
        study = NULL;
        sc_.SetPersistentPointer(1, study);
    }
    return study;
}

void Study::SetReferences(SCStudyInterfaceRef* sc)
{
    _sc = *sc;
}