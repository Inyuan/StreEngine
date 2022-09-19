#pragma once

///configuration
#define CLIENT_WIDTH render_configuration::client_width
#define CLIENT_HEIGHT render_configuration::client_height

#define MSAAX4_STATE render_configuration::msaax4_state
#define MSAAX4_QUALITY render_configuration::msaax4_quality


static class render_configuration
{
public:
	static unsigned int client_width;
	static unsigned int client_height;

	static bool msaax4_state;
	static unsigned int msaax4_quality;
};
unsigned int render_configuration::client_width = 800;
unsigned int render_configuration::client_height = 600;
bool render_configuration::msaax4_state = false;
unsigned int render_configuration::msaax4_quality = 0;


///