#include "table.h"

namespace polaris::tables
{
    void Table::Update()
    {
        if (m_lpPlates.size() > 0)
            m_lpPlates.front()->Update();
    }

    void Table::PushPlate(plates::Plate* plate)
    {
        // NOTE: If there's no plates yet, not having this check will cause a crash.
        if (m_lpPlates.size() > 0)
            m_lpPlates.front()->OnDisabled();

        m_lpPlates.push_front(plate);
        plate->OnEnabled();
    }
    void Table::PopPlate()
    {
        if (m_lpPlates.size() > 0)
        {
            m_lpPlates.front()->OnDisabled();
            m_lpPlates.pop_front();

            if (m_lpPlates.size() > 0)
                m_lpPlates.front()->OnEnabled();
        }
    }
    void Table::Pop()
    {
        m_lpPlates.clear();
        delete this;
    }
}