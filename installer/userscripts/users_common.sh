# shell include script

PATH=/bin:/usr/local/bin:/usr/bin
export PATH

if [ -z "${KOPANO_USER_SCRIPTS}" ] ; then
    exec >&2
    echo "Do not execute this script directly"
    exit 1
fi

if [ ! -d "${KOPANO_USER_SCRIPTS}" ] ; then
    exec >&2
    echo "${KOPANO_USER_SCRIPTS} does not exist or is not a directory"
    exit 1
fi

if [ -z "${KOPANO_USER}" -a -z "${KOPANO_STOREGUID}" ] ; then
    exec >&2
    echo "KOPANO_USER and KOPANO_STOREGUID is not set."
    exit 1
fi
exec "$PKGLIBEXECDIR/kscriptrun" "$KOPANO_USER_SCRIPTS"
