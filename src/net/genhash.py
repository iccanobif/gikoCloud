import sys
import re
root=sys.argv[1]
header='freepoi/net/hash.h'
lookup="freepoi_lookup_string"
struct="freepoi_hash_key"

info=['error', 'status', 'description', 'level', 'code', 'connect']
methods=['StartUpAfterHashCheck',
         'LoginAfterHashCheck',
         'ClientId',
         'IgnoreEach',
         'SystemAlert',
         'TimeMessage',
         'GeneralMessage',
         'Trip',
         'UserLogin',
         'LoginCount',
         'PowerAdmin',
         'PowerGM',
         'Dead',
         'Darkness',
         'EventList',
         'RegisterEvent',
         'DeleteEvent',
         'EventDetail',
         'VoteEvent']

so=['SharedObject.Flush.Failed',
    'SharedObject.Flush.Success',
    'SharedObject.BadPersistence']

ns=['NetStream.Buffer.Empty',
    'NetStream.Buffer.Flush',
    'NetStream.Buffer.Full',
    'NetStream.Connect.Closed',
    'NetStream.Connect.Failed',
    'NetStream.Connect.Rejected',
    'NetStream.Connect.Success',
    'NetStream.DRM.UpdateNeeded',
    'NetStream.Failed',
    'NetStream.MulticastStream.Reset',
    'NetStream.Pause.Notify',
    'NetStream.Play.Failed',
    'NetStream.Play.FileStructureInvalid',
    'NetStream.Play.InsufficientBW',
    'NetStream.Play.NoSupportedTrackFound',
    'NetStream.Play.PublishNotify',
    'NetStream.Play.Reset',
    'NetStream.Play.Start',
    'NetStream.Play.Stop',
    'NetStream.Play.StreamNotFound',
    'NetStream.Play.Transition',
    'NetStream.Play.UnpublishNotify',
    'NetStream.Publish.BadName',
    'NetStream.Publish.Idle',
    'NetStream.Publish.Start',
    'NetStream.Record.AlreadyExists',
    'NetStream.Record.Failed',
    'NetStream.Record.NoAccess',
    'NetStream.Record.Start',
    'NetStream.Record.Stop',
    'NetStream.SecondScreen.Start',
    'NetStream.SecondScreen.Stop',
    'NetStream.Seek.Failed',
    'NetStream.Seek.InvalidTime',
    'NetStream.Seek.Notify',
    'NetStream.Stop.Notify',
    'NetStream.Unpause.Notify',
    'NetStream.Unpublish.Success']

nc=['NetConnection.Connect.AppShutDown',
    'NetConnection.Connect.Closed',
    'NetConnection.Connect.Failed',
    'NetConnection.Connect.IdleTimeout',
    'NetConnection.Connect.InvalidApp',
    'NetConnection.Connect.NetworkChange',
    'NetConnection.Connect.Rejected',
    'NetConnection.Connect.Success',
    'NetConnection.Call.BadVersion',
    'NetConnection.Call.Failed',
    'NetConnection.Call.Prohibited']

enum = list(info)
enum.append('result')
enum.extend([re.sub(r"\B([A-Z])", r"_\1", x).replace('Start_Up', 'STARTUP').replace('_G_M', '_GM') for x in methods])
enum.extend([x.replace('.', '_') for x in nc])
enum.extend([x.replace('.', '_') for x in ns])
enum.extend([x.replace('.', '_') for x in so])

enum = ['FREEPOI_HASH_' + x.upper() for x in enum]

ke=list(info)
ke.append('_result')
ke.extend(['manage' + x for x in methods])
ke.extend(nc)
ke.extend(ns)
ke.extend(so)

ke = [a + ",    " + b for a,b in zip(ke, enum)]

def l2es(l):
    e = "\n"
    for x in l:
        e += "    " + x + ",\n"
    e = e[0:-2] + "\n"
    return e
def l2s(l):
    e = "\n"
    for x in l:
        e += x + "\n"
    return e

enum = l2es(enum)
ke = l2s(ke)
#===========================================
#
#===========================================
htext = """\
#ifndef {0}
#define {0}

#include <freepoi/defs.h>

FREEPOI_BEGIN_EXTERN_C

enum {{{1}}};


struct {2} {{
    const char *name;
    int value;
}};

const struct {2} *
{3}(register const char *str, register unsigned int length);

FREEPOI_END_EXTERN_C

#endif /* {0} */
""".format('FREEPOI_NET_HASH_H', enum, struct, lookup)
#==========================================
#
#==========================================
gtext = """\
%includes
%compare-lengths
%define lookup-function-name {0}
%readonly-tables
%enum
%language=ANSI-C
%omit-struct-type

%{{
#include <{1}>
%}}

struct {2}
%struct-type

%%{3}%%
""".format(lookup, header, struct, ke)

with open(root + 'include/' + header, "wb") as f:
    f.write(htext)

with open(root + 'net/hash.gperf', "wb") as f:
    f.write(gtext)
