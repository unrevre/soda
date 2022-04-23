#include "system.h"

#include <mach/host_info.h>
#include <mach/kern_return.h>
#include <mach/mach_host.h>
#include <mach/mach_init.h>
#include <mach/machine.h>
#include <mach/message.h>
#include <mach/vm_statistics.h>

#include <CoreAudio/AudioHardware.h>
#include <CoreAudio/AudioHardwareBase.h>
#include <CoreAudio/AudioHardwareDeprecated.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFNumber.h>
#include <CoreFoundation/CFString.h>
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>
#include <MacTypes.h>

#include <stddef.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <unistd.h>

int32_t info_bat(void) {
    int i, cur_cap, max_cap;

    cur_cap = 0;
    max_cap = 0;

    CFTypeRef ps_info = IOPSCopyPowerSourcesInfo();
    CFTypeRef ps_list = IOPSCopyPowerSourcesList(ps_info);

    CFIndex ps_count = ps_list ? CFArrayGetCount(ps_list) : 0;

    for (i = 0; i < ps_count; ++i) {
        CFDictionaryRef ps = IOPSGetPowerSourceDescription(
            ps_info, CFArrayGetValueAtIndex(ps_list, i));
        if (!ps) continue;

        CFTypeRef ps_type = CFDictionaryGetValue(ps, CFSTR(kIOPSTypeKey));
        if (!ps_type || !CFEqual(ps_type, CFSTR(kIOPSInternalBatteryType)))
            continue;

        CFTypeRef ps_cur = CFDictionaryGetValue(
            ps, CFSTR(kIOPSCurrentCapacityKey));
        if (!ps_cur) continue;

        CFTypeRef ps_max = CFDictionaryGetValue(
            ps, CFSTR(kIOPSMaxCapacityKey));
        if (!ps_max) continue;

        CFTypeRef ps_charge = CFDictionaryGetValue(
            ps, CFSTR(kIOPSPowerSourceStateKey));
        if (!ps_charge) continue;

        CFNumberGetValue((CFNumberRef)ps_cur, kCFNumberSInt32Type, &cur_cap);
        CFNumberGetValue((CFNumberRef)ps_max, kCFNumberSInt32Type, &max_cap);

        if (ps_list) CFRelease(ps_list);
        if (ps_info) CFRelease(ps_info);

        return cur_cap * 100 / max_cap;
    }

    return 0;
}

int32_t info_vol(void) {
    AudioObjectPropertyAddress property;
    uint32_t size;
    OSStatus result;

    AudioDeviceID did;
    float volume;

    property = (AudioObjectPropertyAddress) {
        kAudioHardwarePropertyDefaultOutputDevice,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster,
    };

    size = sizeof(AudioDeviceID);

    result = AudioObjectGetPropertyData(
        kAudioObjectSystemObject, &property, 0, NULL, &size, &did);

    if (kAudioHardwareNoError != result)
        return 0;

    property = (AudioObjectPropertyAddress) {
        kAudioDevicePropertyVolumeScalar,
        kAudioDevicePropertyScopeOutput,
        1,
    };

    if (!AudioObjectHasProperty(did, &property))
        return 0;

    size = sizeof(float);

    result = AudioObjectGetPropertyData(
        did, &property, 0, NULL, &size, &volume);

    if (kAudioHardwareNoError != result)
        return 0;

    return volume * 100;
}

int32_t info_cpu(void) {
    host_cpu_load_info_data_t cpuinfo;
    mach_msg_type_number_t count;

    int64_t ticks, total, idle;
    int32_t i;

    count = HOST_CPU_LOAD_INFO_COUNT;
    total = 0, idle = 0;

    if (host_statistics(mach_host_self(),
                        HOST_CPU_LOAD_INFO,
                        (host_info_t)&cpuinfo,
                        &count) == KERN_SUCCESS) {
        for (i = 0, ticks = 0; i < CPU_STATE_MAX; ++i)
            ticks += cpuinfo.cpu_ticks[i];

        total = ticks;
        idle = cpuinfo.cpu_ticks[CPU_STATE_IDLE];
    } else {
        return 0;
    }

    sleep(1);

    if (host_statistics(mach_host_self(),
                        HOST_CPU_LOAD_INFO,
                        (host_info_t)&cpuinfo,
                        &count) == KERN_SUCCESS) {
        for (i = 0, ticks = 0; i < CPU_STATE_MAX; ++i)
            ticks += cpuinfo.cpu_ticks[i];

        total = ticks - total;
        idle = cpuinfo.cpu_ticks[CPU_STATE_IDLE] - idle;

        return (total - idle) * 100 / total;
    }

    return 0;
}

int32_t info_ram(void) {
    mach_port_t mach_port;
    mach_msg_type_number_t count;
    vm_size_t page_size;
    vm_statistics64_data_t vm_stats;

    uint32_t free_memory, used_memory;

    mach_port = mach_host_self();
    count = sizeof(vm_stats) / sizeof(natural_t);

    if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
        KERN_SUCCESS == host_statistics64(mach_port,
                                          HOST_VM_INFO,
                                          (host_info64_t)&vm_stats,
                                          &count)) {

        free_memory = vm_stats.free_count;
        used_memory = vm_stats.active_count
            + vm_stats.inactive_count
            + vm_stats.wire_count;

        return used_memory * 100 / (used_memory + free_memory);
    }

    return 0;
}

int32_t info_hdd(void) {
    struct statvfs st;

    statvfs("/Users", &st);

    return (st.f_blocks - st.f_bfree) * 100 / st.f_blocks;
}
