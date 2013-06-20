#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#define stringify( name ) # name


namespace iviLink
{
namespace SystemController
{

enum Components
{
    PROFILE_MANAGER = 0,
    CHANNEL_SUPERVISOR,
    CONNECTIVITY_AGENT,
    APPLICATION_MANAGER,
    AUTHENTICATION_APP,
    SPLASH_SCREEN,
    SYSTEM_CONTROLLER,
    SYSTEM_CONTROLLER_WATCHDOG,
};

static const char* ComponentsNames[] = 
{
    stringify(PROFILE_MANAGER),
    stringify(CHANNEL_SUPERVISOR),
    stringify(CONNECTIVITY_AGENT),
    stringify(APPLICATION_MANAGER),
    stringify(AUTHENTICATION_APP),
    stringify(SPLASH_SCREEN),
    stringify(SYSTEM_CONTROLLER)
};

} // namespace SystemController
}

#endif //COMPONENTS_HPP