#include "ChangePlayerForm.hpp"
#include <helper.hpp>
#include "kh2-dmenu.hpp"
#include <Entity.hpp>
#include <hook_directx12.hpp>

ChangePlayerForm::ChangePlayerForm(OBJENTRY_ID formId)
    : m_formId(formId)
{
}

void ChangePlayerForm::Activate() {
    /*Entity* player = GetPlayer1();
    if (!player) {
        LOG("ChangePlayerForm: Player not found!");
        return;
    }*/

    
    NewCapture(m_formId); 

    LOG("Changed player form to ID %d", m_formId);
}

std::string ChangePlayerForm::GetName() const {
    return "Change Player Form";
}
