#include <cmw/transport/transport.h>


namespace hnu    {
namespace cmw   {
namespace transport {

Transport::Transport()
{
    CreateParticipant();
}

Transport::~Transport() { Shutdown(); }

void Transport::Shutdown()
{
    if(is_shutdown_.exchange(true)){
        return ;
    }

    if(participant_ != nullptr)
    {
        participant_->Shutdown();
    }
    participant_ = nullptr;
}

void Transport::CreateParticipant(){

    std::string participant_name = 
            common::GlobalData::Instance()->HostName() + "+" + 
            std::to_string(common::GlobalData::Instance()->ProcessId());
    participant_ = std::make_shared<Participant>(participant_name , 11512);
}

}
}
}