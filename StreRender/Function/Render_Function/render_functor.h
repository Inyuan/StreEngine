#include <queue>
#include <functional>
#include "stre_render_api.h"

typedef std::function<void(s_directx_render* in_render)>
dx_function;


//functor
template<class t_function>
class function_command
{
public:

	std::queue<t_function> command_queue;
};

static function_command<dx_function> dx_pass_command;

static function_command<dx_function> dx_shader_resource_command;
