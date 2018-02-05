#pragma once

#include "UnrealEd.h"
#include "PropertyEditing.h"
#include "PropertyCustomizationHelpers.h"


#define UNIMPLEMENTED_MESSAGE FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("UnimplementedError", "This functionality is not yet implemented"));