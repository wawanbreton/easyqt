#pragma once

#include <QtCore/qglobal.h>

#if defined(EASYQT_LIBRARY)
#define EASYQT_EXPORT Q_DECL_EXPORT
#else
#define EASYQT_EXPORT Q_DECL_IMPORT
#endif
