#ifndef MYTMSUUI_CONSTS_H
#define MYTMSUUI_CONSTS_H

#include <QString>

namespace MyTMSUUI_Consts
{
   static const QString APP_NAME_DISPLAY = "My TMSU UI";
   static const QString APP_NAME_CLI = "my-tmsu-ui";
   static const QString APP_VERSION = "0.1.0";
   static const QString APP_DESCRIPTION = "A GUI Frontend for TMSU";
   static const QString APP_AUTHOR = "Jess Odum";
   static const QString ABOUT_TITLE = "About " + APP_NAME_DISPLAY;
   static const QString HELP_DOC_URL = "https://github.com/jessodum/my-tmsu-ui/blob/main/README.md";
   static const QString UA_TAGS_DIAG_TITLE = "Un-applied changes";
   static const QString UA_TAGS_DIAG_TEXT = "There are tag updates that have not yet been applied. Do you want to apply them now?";
   static const QString UA_TAGS_DIAG_CANNOT_CANCEL = "NOTE: The action you performed cannot be canceled.";
}

#endif
