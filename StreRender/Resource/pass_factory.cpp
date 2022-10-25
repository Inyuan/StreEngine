#include "stre_render.h"
#include "Function/Render_Function/render_system.h"

//创建pass
template<class t_render>
s_pass* pass_factory::create_pass()
{
	auto labdma = [](
		t_render* render)
	{
		return render->allocate_pass();
		//...
	}

	render_command<t_render>::command_queue.push();
}


void pass_factory::add_mesh(
	s_pass* in_out_pass, 
	const cpu_mesh*  in_mesh)
{
	//可以喊别的线程做
	auto labdma = [in_out_pass, in_mesh](
		s_pass* in_out_pass,
		const cpu_mesh* in_mesh)
	{
		s_pass::mesh_resource g_mesh;
		g_mesh.gpu_vertex_layout.vertex = in_mesh->vertex_ptr->gpu_sr_ptr;
		g_mesh.gpu_vertex_layout.index = in_mesh->index_ptr->gpu_sr_ptr;

		g_mesh.gpu_mesh_resource_ptr[in_mesh->object_constant_ptr->uid.name] = (in_mesh->object_constant_ptr->gpu_sr_ptr);
		g_mesh.gpu_mesh_resource_ptr[in_mesh->material_ptr->uid.name] = (in_mesh->material_ptr->gpu_sr_ptr);
		for (int i = 0; i < in_mesh->texture_ptr.size(); i++)
		{
			g_mesh.gpu_mesh_resource_ptr[in_mesh->texture_ptr[i]->uid.name] = (in_mesh->texture_ptr[i]->gpu_sr_ptr);
		}

		in_out_pass->gpu_mesh[in_mesh->uid.name] = g_mesh;
		//...
	};

	render_command<t_render>::command_queue.push(labdma);
}

template<class t_cpu_resource>
void pass_factory::add_shader_resource(
	s_pass* in_out_pass,
	const t_cpu_resource* in_sr)
{
	//可以喊别的线程做
	auto labdma = [
		in_out_pass, 
			in_sr->uid,
			in_sr->gpu_sr_ptr](
		s_pass* in_out_pass,
		const s_uid in_uid,
		const gpu_shader_resource* in_gpu_sr)
	{
		in_out_pass->gpu_pass_resource_ptr[in_uid.name] = in_gpu_sr;
		//...
	};

	render_command<t_render>::command_queue.push(labdma);
}

void pass_factory::add_render_target(
	s_pass* in_out_pass,
	const gpu_rander_target* in_gpu_rt)
{
	//可以喊别的线程做
	auto labdma = [
		in_out_pass,
			in_gpu_rt->uid,
			in_gpu_rt->gpu_sr_ptr](
		s_pass* in_out_pass,
		const s_uid in_uid,
		const gpu_shader_resource* in_gpu_rt)
	{
			in_out_pass->gpu_rt_texture_ptr[in_uid.name] = in_gpu_rt;
		//...
	};

	render_command<t_render>::command_queue.push(labdma);
}

void pass_factory::remove_mesh(
	s_pass* in_out_pass,
	const cpu_mesh* in_mesh)
{
	//可以喊别的线程做
	auto labdma = [in_out_pass,in_mesh](
		s_pass* in_out_pass,
		const cpu_mesh* in_mesh)
	{
		in_out_pass->gpu_mesh.erase(in_mesh->uid.name);

		//...
	};

}
template<class t_cpu_resource>
void pass_factory::remove_shader_resource(
	s_pass* in_out_pass,
	const t_cpu_resource* in_sr)
{
	//可以喊别的线程做
	auto labdma = [in_out_pass,
		in_sr->uid](
		s_pass* in_out_pass,
		s_uid in_uid)
	{
		in_out_pass->gpu_pass_resource_ptr.erase(in_uid.name);

		//...
	};

}

void pass_factory::remove_render_target(
	s_pass* in_out_pass,
	const gpu_rander_target* in_gpu_rt)
{
	//可以喊别的线程做
	auto labdma = [in_out_pass,
		in_gpu_rt->uid](
		s_pass* in_out_pass,
		const s_uid in_uid)
	{
		in_out_pass->gpu_rt_texture_ptr.erase(in_uid.name);

		//...
	};

}