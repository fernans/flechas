@echo off

rem Remove existing executables

del /Q object_lists\add_landmarks_to_objects.exe object_lists\crop_objects_from_lists.exe object_lists\crop_thumbnails_from_lists.exe object_lists\overlay_object_lists.exe object_lists\print_object_list.exe 2>nul

del /Q completed_track_lists\get_dispersed_tracks_from_completed_tracks.exe completed_track_lists\overlay_completed_track_list.exe completed_track_lists\print_completed_track_list.exe completed_track_lists\print_completed_track_list_summaries.exe 2>nul

del /Q tracked_object_lists\print_tracked_object_list.exe 2>nul

del /Q templates\compare_template_to_gallery.exe templates\extract_templates_from_object_lists.exe templates\get_template_string.exe templates\get_template_thumbnails.exe templates\set_template_string.exe templates\strip_template_thumbnail.exe 2>nul

del /Q galleries\add_templates_to_gallery.exe galleries\clear_template_relationships.exe galleries\compare_galleries.exe galleries\create_empty_gallery.exe galleries\get_all_template_strings.exe galleries\get_all_template_thumbnails.exe galleries\get_all_templates.exe galleries\get_gallery_string.exe galleries\get_num_templates_in_gallery.exe galleries\get_self_similarity_matrix.exe galleries\get_template_by_id.exe galleries\get_template_id_list_from_gallery.exe galleries\get_template_string_by_id.exe galleries\get_template_thumbnail_by_id.exe galleries\get_user_specified_template_relationship.exe galleries\get_user_specified_template_relationship_list.exe galleries\get_user_specified_template_relationship_matrix.exe galleries\print_gallery.exe galleries\remove_template_from_gallery.exe galleries\set_gallery_string.exe galleries\set_template_relationship.exe galleries\set_template_string_by_id.exe galleries\strip_all_template_thumbnails.exe galleries\trim_subject_to_representative_templates 2>nul

del /Q gallery_groups\add_galleries_to_gallery_group.exe gallery_groups\cluster_gallery_group.exe gallery_groups\create_empty_gallery_group.exe gallery_groups\get_all_galleries.exe gallery_groups\get_all_gallery_strings.exe gallery_groups\get_gallery_by_id.exe gallery_groups\get_gallery_id_list_from_gallery_group.exe gallery_groups\get_gallery_string_by_id.exe gallery_groups\get_num_galleries_in_gallery_group.exe gallery_groups\get_ranked_list_for_template_source.exe gallery_groups\get_user_specified_template_source_relationship.exe gallery_groups\remove_gallery_from_gallery_group.exe gallery_groups\set_gallery_string_by_id.exe gallery_groups\set_template_source_relationship.exe 2>nul

del /Q similarity_matrices\get_query_template_id_list_from_similarity_matrix.exe similarity_matrices\get_similarity_matrix_col_by_id.exe similarity_matrices\get_similarity_matrix_col_by_index.exe similarity_matrices\get_similarity_matrix_data.exe similarity_matrices\get_similarity_matrix_dimensions.exe similarity_matrices\get_similarity_matrix_element_by_id.exe similarity_matrices\get_similarity_matrix_element_by_index.exe similarity_matrices\get_similarity_matrix_row_by_id.exe similarity_matrices\get_similarity_matrix_row_by_index.exe similarity_matrices\get_target_template_id_list_from_similarity_matrix.exe similarity_matrices\print_similarity_matrix.exe 2>nul

del /Q raw_image\convert_image.exe raw_image\overlay_text_on_image.exe 2>nul

del /Q video\clip_video.exe video\create_video.exe video\extract_frames.exe video\get_frame_rate.exe video\overlay_frame_numbers_on_video.exe 2>nul

del /Q miscellaneous\pixels_to_size.exe miscellaneous\version.exe 2>nul


rem Build Object List Tools
echo. 
echo Building object list tools
echo. 

cl ..\source\tools\object_lists\add_landmarks_to_objects.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Feobject_lists\add_landmarks_to_objects.exe 

cl ..\source\tools\object_lists\crop_objects_from_lists.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Feobject_lists\crop_objects_from_lists.exe 

cl ..\source\tools\object_lists\crop_thumbnails_from_lists.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Feobject_lists\crop_thumbnails_from_lists.exe 

cl ..\source\tools\object_lists\overlay_object_lists.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Feobject_lists\overlay_object_lists.exe 

cl ..\source\tools\object_lists\print_object_list.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Feobject_lists\print_object_list.exe 

xcopy /Q /Y ..\lib\pittpatt_ftr_sdk.dll       object_lists\.
xcopy /Q /Y ..\lib\pittpatt_raw_image.dll     object_lists\.
xcopy /Q /Y ..\lib\pittpatt_raw_image_io.dll  object_lists\.
xcopy /Q /Y ..\lib\pittpatt_video_io.dll      object_lists\.


rem Build Completed Track List Tools
echo.
echo Building completed track list tools
echo.

cl ..\source\tools\completed_track_lists\get_dispersed_tracks_from_completed_tracks.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fecompleted_track_lists\get_dispersed_tracks_from_completed_tracks.exe

cl ..\source\tools\completed_track_lists\overlay_completed_track_list.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fecompleted_track_lists\overlay_completed_track_list.exe 

cl ..\source\tools\completed_track_lists\print_completed_track_list.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fecompleted_track_lists\print_completed_track_list.exe

cl ..\source\tools\completed_track_lists\print_completed_track_list_summaries.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fecompleted_track_lists\print_completed_track_list_summaries.exe

xcopy /Q /Y ..\lib\pittpatt_ftr_sdk.dll       completed_track_lists\.
xcopy /Q /Y ..\lib\pittpatt_raw_image.dll     completed_track_lists\.
xcopy /Q /Y ..\lib\pittpatt_raw_image_io.dll  completed_track_lists\.
xcopy /Q /Y ..\lib\pittpatt_video_io.dll      completed_track_lists\.


rem Build Tracked Object List Tools
echo.
echo Building tracked object list tools
echo.

cl ..\source\tools\tracked_object_lists\print_tracked_object_list.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fetracked_object_lists\print_tracked_object_list.exe

xcopy /Q /Y ..\lib\pittpatt_ftr_sdk.dll       tracked_object_lists\.
xcopy /Q /Y ..\lib\pittpatt_raw_image.dll     tracked_object_lists\.
xcopy /Q /Y ..\lib\pittpatt_raw_image_io.dll  tracked_object_lists\.
xcopy /Q /Y ..\lib\pittpatt_video_io.dll      tracked_object_lists\.


rem Build Template Tools
echo.
echo Building template tools
echo.

cl ..\source\tools\templates\compare_template_to_gallery.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fetemplates\compare_template_to_gallery.exe

cl ..\source\tools\templates\extract_templates_from_object_lists.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fetemplates\extract_templates_from_object_lists.exe

cl ..\source\tools\templates\get_template_string.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fetemplates\get_template_string.exe

cl ..\source\tools\templates\get_template_thumbnails.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fetemplates\get_template_thumbnails.exe

cl ..\source\tools\templates\set_template_string.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fetemplates\set_template_string.exe

cl ..\source\tools\templates\strip_template_thumbnail.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fetemplates\strip_template_thumbnail.exe

xcopy /Q /Y ..\lib\pittpatt_ftr_sdk.dll       templates\.
xcopy /Q /Y ..\lib\pittpatt_raw_image.dll     templates\.
xcopy /Q /Y ..\lib\pittpatt_raw_image_io.dll  templates\.
xcopy /Q /Y ..\lib\pittpatt_video_io.dll      templates\.


rem Build Gallery Tools
echo.
echo Building gallery tools
echo.

cl ..\source\tools\galleries\add_templates_to_gallery.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\add_templates_to_gallery.exe

cl ..\source\tools\galleries\compare_galleries.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\compare_galleries.exe 

cl ..\source\tools\galleries\clear_template_relationships.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\clear_template_relationships.exe 

cl ..\source\tools\galleries\create_empty_gallery.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\create_empty_gallery.exe 

cl ..\source\tools\galleries\get_all_template_strings.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\get_all_template_strings.exe 

cl ..\source\tools\galleries\get_all_template_thumbnails.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\get_all_template_thumbnails.exe 

cl ..\source\tools\galleries\get_all_templates.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\get_all_templates.exe 

cl ..\source\tools\galleries\get_gallery_string.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\get_gallery_string.exe 

cl ..\source\tools\galleries\get_num_templates_in_gallery.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\get_num_templates_in_gallery.exe 

cl ..\source\tools\galleries\get_self_similarity_matrix.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\get_self_similarity_matrix.exe 

cl ..\source\tools\galleries\get_template_by_id.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\get_template_by_id.exe 

cl ..\source\tools\galleries\get_template_id_list_from_gallery.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\get_template_id_list_from_gallery.exe 

cl ..\source\tools\galleries\get_template_string_by_id.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\get_template_string_by_id.exe 

cl ..\source\tools\galleries\get_template_thumbnail_by_id.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\get_template_thumbnail_by_id.exe 

cl ..\source\tools\galleries\get_user_specified_template_relationship.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\get_user_specified_template_relationship.exe 

cl ..\source\tools\galleries\get_user_specified_template_relationship_list.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\get_user_specified_template_relationship_list.exe 

cl ..\source\tools\galleries\get_user_specified_template_relationship_matrix.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\get_user_specified_template_relationship_matrix.exe 

cl ..\source\tools\galleries\print_gallery.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\print_gallery 

cl ..\source\tools\galleries\remove_template_from_gallery.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\remove_template_from_gallery.exe

cl ..\source\tools\galleries\set_gallery_string.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\set_gallery_string.exe

cl ..\source\tools\galleries\set_template_relationship.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\set_template_relationship.exe

cl ..\source\tools\galleries\set_template_string_by_id.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\set_template_string_by_id.exe

cl ..\source\tools\galleries\strip_all_template_thumbnails.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\strip_all_template_thumbnails.exe 

cl ..\source\tools\galleries\trim_subject_to_representative_templates.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegalleries\trim_subject_to_representative_templates.exe

xcopy /Q /Y ..\lib\pittpatt_ftr_sdk.dll       galleries\.
xcopy /Q /Y ..\lib\pittpatt_raw_image.dll     galleries\.
xcopy /Q /Y ..\lib\pittpatt_raw_image_io.dll  galleries\.
xcopy /Q /Y ..\lib\pittpatt_video_io.dll      galleries\.


rem Build Gallery Group Tools
echo.
echo Building gallery group tools
echo.

cl ..\source\tools\gallery_groups\add_galleries_to_gallery_group.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\add_galleries_to_gallery_group.exe

cl ..\source\tools\gallery_groups\cluster_gallery_group.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\cluster_gallery_group.exe

cl ..\source\tools\gallery_groups\create_empty_gallery_group.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\create_empty_gallery_group.exe 

cl ..\source\tools\gallery_groups\get_all_galleries.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\get_all_galleries.exe 

cl ..\source\tools\gallery_groups\get_all_gallery_strings.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\get_all_gallery_strings.exe 

cl ..\source\tools\gallery_groups\get_gallery_by_id.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\get_gallery_by_index.exe 

cl ..\source\tools\gallery_groups\get_gallery_id_list_from_gallery_group.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\get_gallery_id_list_from_gallery_group.exe 

cl ..\source\tools\gallery_groups\get_gallery_string_by_id.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\get_gallery_string_by_id.exe 

cl ..\source\tools\gallery_groups\get_num_galleries_in_gallery_group.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\get_num_galleries_in_gallery_group.exe 

cl ..\source\tools\gallery_groups\get_ranked_list_for_template_source.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\get_ranked_list_for_template_source.exe

cl ..\source\tools\gallery_groups\get_user_specified_template_source_relationship.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\get_user_specified_template_source_relationship.exe

cl ..\source\tools\gallery_groups\get_user_specified_template_source_relationship.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\get_user_specified_template_source_relationship.exe

cl ..\source\tools\gallery_groups\remove_gallery_from_gallery_group.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\remove_gallery_from_gallery_group.exe

cl ..\source\tools\gallery_groups\set_gallery_string_by_id.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\set_gallery_string_by_id.exe 

cl ..\source\tools\gallery_groups\set_template_source_relationship.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fegallery_groups\set_template_source_relationship.exe 

xcopy /Q /Y ..\lib\pittpatt_ftr_sdk.dll       gallery_groups\.
xcopy /Q /Y ..\lib\pittpatt_raw_image.dll     gallery_groups\.
xcopy /Q /Y ..\lib\pittpatt_raw_image_io.dll  gallery_groups\.
xcopy /Q /Y ..\lib\pittpatt_video_io.dll      gallery_groups\.


rem Build Similarity Matrix Tools
echo.
echo Building similarity matrix tools
echo.

cl ..\source\tools\similarity_matrices\get_target_template_id_list_from_similarity_matrix.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fesimilarity_matrices\get_target_template_id_list_from_similarity_matrix.exe 

cl ..\source\tools\similarity_matrices\get_query_template_id_list_from_similarity_matrix.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fesimilarity_matrices\get_query_template_id_list_from_similarity_matrix.exe 

cl ..\source\tools\similarity_matrices\get_similarity_matrix_col_by_id.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fesimilarity_matrices\get_similarity_matrix_col_by_id.exe 

cl ..\source\tools\similarity_matrices\get_similarity_matrix_col_by_index.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fesimilarity_matrices\get_similarity_matrix_col_by_index.exe

cl ..\source\tools\similarity_matrices\get_similarity_matrix_data.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fesimilarity_matrices\get_similarity_matrix_data.exe 

cl ..\source\tools\similarity_matrices\get_similarity_matrix_dimensions.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fesimilarity_matrices\get_similarity_matrix_dimensions.exe 

cl ..\source\tools\similarity_matrices\get_similarity_matrix_element_by_id.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fesimilarity_matrices\get_similarity_matrix_element_by_id.exe 

cl ..\source\tools\similarity_matrices\get_similarity_matrix_element_by_index.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fesimilarity_matrices\get_similarity_matrix_element_by_index.exe 

cl ..\source\tools\similarity_matrices\get_similarity_matrix_row_by_id.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fesimilarity_matrices\get_similarity_matrix_row_by_id.exe 

cl ..\source\tools\similarity_matrices\get_similarity_matrix_row_by_index.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fesimilarity_matrices\get_similarity_matrix_row_by_index.exe 

cl ..\source\tools\similarity_matrices\print_similarity_matrix.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fesimilarity_matrices\print_similarity_matrix.exe

xcopy /Q /Y ..\lib\pittpatt_ftr_sdk.dll       similarity_matrices\.
xcopy /Q /Y ..\lib\pittpatt_raw_image.dll     similarity_matrices\.
xcopy /Q /Y ..\lib\pittpatt_raw_image_io.dll  similarity_matrices\.
xcopy /Q /Y ..\lib\pittpatt_video_io.dll      similarity_matrices\.


rem Build Raw Image Tools
echo.
echo Building raw image tools
echo.

cl ..\source\tools\raw_image\convert_image.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Feraw_image\convert_image.exe

cl ..\source\tools\raw_image\overlay_text_on_image.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Feraw_image\overlay_text_on_image.exe

xcopy /Q /Y ..\lib\pittpatt_ftr_sdk.dll       raw_image\.
xcopy /Q /Y ..\lib\pittpatt_raw_image.dll     raw_image\.
xcopy /Q /Y ..\lib\pittpatt_raw_image_io.dll  raw_image\.
xcopy /Q /Y ..\lib\pittpatt_video_io.dll      raw_image\.


rem Build Video Tools
echo.
echo Building video tools
echo.

cl ..\source\tools\video\clip_video.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fevideo\clip_video.exe

cl ..\source\tools\video\create_video.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fevideo\create_video.exe

cl ..\source\tools\video\extract_frames.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fevideo\extract_frames.exe

cl ..\source\tools\video\get_frame_rate.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fevideo\get_frame_rate.exe

cl ..\source\tools\video\overlay_frame_numbers_on_video.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Fevideo\overlay_frame_numbers_on_video.exe

xcopy /Q /Y ..\lib\pittpatt_ftr_sdk.dll       video\.
xcopy /Q /Y ..\lib\pittpatt_raw_image.dll     video\.
xcopy /Q /Y ..\lib\pittpatt_raw_image_io.dll  video\.
xcopy /Q /Y ..\lib\pittpatt_video_io.dll      video\.


rem Build Miscellaneous Tools
echo.
echo Building miscellaneous tools
echo.

cl ..\source\tools\miscellaneous\pixels_to_size.c ..\source\utilities\sdk_utils.c ..\source\utilities\sdk_draw_utils.c -I..\include -I..\source\utilities -I..\source\common ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image_io.lib ..\lib\pittpatt_video_io.lib ..\lib\pittpatt_raw_image.lib -nologo -Femiscellaneous\pixels_to_size.exe

cl ..\source\tools\miscellaneous\version.c -I..\include ..\lib\pittpatt_ftr_sdk.lib ..\lib\pittpatt_raw_image.lib -nologo -Femiscellaneous\version.exe

xcopy /Q /Y ..\lib\pittpatt_ftr_sdk.dll       miscellaneous\.
xcopy /Q /Y ..\lib\pittpatt_raw_image.dll     miscellaneous\.
xcopy /Q /Y ..\lib\pittpatt_raw_image_io.dll  miscellaneous\.
xcopy /Q /Y ..\lib\pittpatt_video_io.dll      miscellaneous\.


rem Clean up

del /Q *.obj
