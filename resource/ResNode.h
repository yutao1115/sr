#pragma once

#include "Id.h"
#include "ResourceState.h"
struct ResNode{
    Id     id_ ;
    ResourceState::Code resState_ = ResourceState::Code::InvalidState;
};