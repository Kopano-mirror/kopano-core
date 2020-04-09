/*
 * SPDX-License-Identifier: AGPL-3.0-only
 * Copyright 2005 - 2016 Zarafa and its licensors
 */
#pragma once

// For PR_MESSAGE_FLAGS
#define MSGFLAG_DELETED				((ULONG) 0x00000400)
#define MSGFLAG_NOTIFY_FLAGS		(MSGFLAG_DELETED | MSGFLAG_ASSOCIATED)
#define MSGFLAG_SETTABLE_BY_USER	(MSGFLAG_UNMODIFIED| MSGFLAG_READ | MSGFLAG_UNSENT | MSGFLAG_FROMME | MSGFLAG_RESEND)
#define MSGFLAG_SETTABLE_BY_SPOOLER	(MSGFLAG_RN_PENDING| MSGFLAG_NRN_PENDING)
#define MSGFLAG_UNSETTABLE			(MSGFLAG_DELETED | MSGFLAG_ASSOCIATED)
