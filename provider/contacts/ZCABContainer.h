/*
 * SPDX-License-Identifier: AGPL-3.0-only
 * Copyright 2005 - 2016 Zarafa and its licensors
 */
#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include <kopano/memory.hpp>
#include <kopano/Util.h>
#include <kopano/zcdefs.h>
#include <mapispi.h>
#include <mapidefs.h>
#include "ZCABLogon.h"
#include "ZCABData.h"
#include "ZCMAPIProp.h"

/* should be derived from IMAPIProp, but since we don't do anything with those functions, let's skip the red tape. */
class ZCABContainer KC_FINAL_OPG :
    public KC::ECUnknown, public IABContainer, public IDistList {
protected:
	ZCABContainer(std::shared_ptr<std::vector<zcabFolderEntry>> folders, IMAPIFolder *contacts, IMAPISupport *, void *provider);

private:
	HRESULT MakeWrappedEntryID(ULONG cbEntryID, LPENTRYID lpEntryID, ULONG ulObjType, ULONG ulOffset, ULONG *lpcbEntryID, LPENTRYID *lppEntryID);

public:
	static HRESULT Create(std::shared_ptr<std::vector<zcabFolderEntry>> folders, IMAPIFolder *contacts, IMAPISupport *, void *provider, ZCABContainer **);
	static HRESULT Create(IMessage *contact, ULONG eid_size, const ENTRYID *eid, IMAPISupport *, ZCABContainer **);

	HRESULT GetFolderContentsTable(ULONG ulFlags, LPMAPITABLE *lppTable);
	HRESULT GetDistListContentsTable(ULONG ulFlags, LPMAPITABLE *lppTable);

	// IUnknown
	virtual HRESULT	QueryInterface(const IID &, void **) override;

	// IABContainer
	virtual HRESULT ResolveNames(const SPropTagArray *props, unsigned int flags, ADRLIST *, FlagList *) override;

	// From IMAPIContainer
	virtual HRESULT GetContentsTable(unsigned int flags, IMAPITable **) override;
	virtual HRESULT GetHierarchyTable(unsigned int flags, IMAPITable **) override;
	virtual HRESULT OpenEntry(unsigned int eid_size, const ENTRYID *eid, const IID *intf, unsigned int flags, unsigned int *obj_type, IUnknown **) override;

	// very limited IMAPIProp, passed to ZCMAPIProp for m_lpDistList.
	virtual HRESULT GetProps(const SPropTagArray *, unsigned int flags, unsigned int *nvals, SPropValue **) override;
	virtual HRESULT GetPropList(unsigned int flags, SPropTagArray **) override;

private:
	std::shared_ptr<std::vector<zcabFolderEntry>> m_lpFolders;
	KC::object_ptr<IMAPIFolder> m_lpContactFolder;
	KC::object_ptr<IMAPISupport> m_lpMAPISup;
	void *m_lpProvider;
	std::mutex m_storemap_lock;

	/* distlist version of this container */
	KC::object_ptr<IMAPIProp> m_lpDistList;
	ALLOC_WRAP_FRIEND;
};
