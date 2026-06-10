# @claudius category: animation_studio
# @claudius description: Queue a Level Sequence for rendering through the Movie Render Queue with a named preset. Output goes to {Project}/Saved/MovieRenders/<sequence_name>/.
# @claudius workflow: animation, shot, rendering

import os
import unreal


def execute(params):
    sequence_path = params.get("sequence_path")
    preset_path = params.get("preset_path")  # Optional MoviePipelinePrimaryConfig asset
    output_subfolder = params.get("output_subfolder")  # Optional, defaults to sequence name

    if not sequence_path:
        return {"success": False, "message": "Missing 'sequence_path' parameter"}

    sequence = unreal.load_asset(sequence_path)
    if not sequence:
        return {"success": False, "message": f"Sequence not found: {sequence_path}"}

    subsystem = unreal.get_editor_subsystem(unreal.MoviePipelineQueueSubsystem)
    if not subsystem:
        return {"success": False, "message": "MoviePipelineQueueSubsystem unavailable (check MovieRenderPipeline plugin)"}

    queue = subsystem.get_queue()
    queue.delete_all_jobs()

    job = queue.allocate_new_job(unreal.MoviePipelineExecutorJob)
    job.sequence = unreal.SoftObjectPath(sequence_path)
    job.map = unreal.SoftObjectPath(unreal.EditorLevelLibrary.get_editor_world().get_path_name())

    # Apply preset if provided, else fall back to a default config
    if preset_path:
        preset = unreal.load_asset(preset_path)
        if not preset:
            return {"success": False, "message": f"Preset not found: {preset_path}"}
        job.set_configuration(preset)
    else:
        cfg = job.get_configuration()
        cfg.find_or_add_setting_by_class(unreal.MoviePipelineDeferredPassBase)
        out = cfg.find_or_add_setting_by_class(unreal.MoviePipelineImageSequenceOutput_PNG)
        anti = cfg.find_or_add_setting_by_class(unreal.MoviePipelineAntiAliasingSetting)
        anti.spatial_sample_count = 2

    seq_name = sequence_path.split("/")[-1].split(".")[0]
    folder = output_subfolder or seq_name
    output_path = os.path.join(unreal.SystemLibrary.get_project_saved_directory(), "MovieRenders", folder)

    out_settings = job.get_configuration().find_or_add_setting_by_class(unreal.MoviePipelineOutputSetting)
    out_settings.output_directory = unreal.DirectoryPath(output_path)
    out_settings.file_name_format = "{sequence_name}.{frame_number}"

    executor = subsystem.render_queue_with_executor(unreal.MoviePipelinePIEExecutor)

    return {
        "success": True,
        "message": f"Queued render for {seq_name}; output to {output_path}",
        "output": {
            "sequence": sequence_path,
            "output_directory": output_path,
            "preset": preset_path or "(default)",
        },
    }
