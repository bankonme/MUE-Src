// Copyright (c) 2009-2015 Bitcoin Developers
// Copyright (c) 2014-2015 MonetaryUnit Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "macnotificationhandler.h"

#undef slots
#include <Cocoa/Cocoa.h>

void MacNotificationHandler::showNotification(const QString &title, const QString &text)
{
    // check if users OS has support for NSUserNotification
    if(this->hasUserNotificationCenterSupport()) {
        // okay, seems like 10.8+
        QByteArray utf8 = title.toUtf8();
        char* cString = (char *)utf8.constData();
        NSString *titleMac = [[NSString alloc] initWithUTF8String:cString];

        utf8 = text.toUtf8();
        cString = (char *)utf8.constData();
        NSString *textMac = [[NSString alloc] initWithUTF8String:cString];

        // do everything weak linked (because we will keep <10.8 compatibility)
        id userNotification = [[NSClassFromString(@"NSUserNotification") alloc] init];
        [userNotification performSelector:@selector(setTitle:) withObject:titleMac];
        [userNotification performSelector:@selector(setInformativeText:) withObject:textMac];

        id notificationCenterInstance = [NSClassFromString(@"NSUserNotificationCenter") performSelector:@selector(defaultUserNotificationCenter)];
        [notificationCenterInstance performSelector:@selector(deliverNotification:) withObject:userNotification];

        [titleMac release];
        [textMac release];
        [userNotification release];
    }
}

// sendAppleScript just take a QString and executes it as apple script
void MacNotificationHandler::sendAppleScript(const QString &script)
{
    QByteArray utf8 = script.toUtf8();
    char* cString = (char *)utf8.constData();
    NSString *scriptApple = [[NSString alloc] initWithUTF8String:cString];

    NSAppleScript *as = [[NSAppleScript alloc] initWithSource:scriptApple];
    NSDictionary *err = nil;
    [as executeAndReturnError:&err];
    [as release];
    [scriptApple release];
}

bool MacNotificationHandler::hasUserNotificationCenterSupport(void)
{
    Class possibleClass = NSClassFromString(@"NSUserNotificationCenter");

    // check if users OS has support for NSUserNotification
    if(possibleClass!=nil) {
        return true;
    }
    return false;
}


MacNotificationHandler *MacNotificationHandler::instance()
{
    static MacNotificationHandler *s_instance = NULL;
    if (!s_instance)
        s_instance = new MacNotificationHandler();
    return s_instance;
}
