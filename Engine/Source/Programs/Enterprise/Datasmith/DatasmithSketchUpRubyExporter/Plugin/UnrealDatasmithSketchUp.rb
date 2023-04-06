#Copyright Epic Games, Inc. All Rights Reserved.

require 'sketchup.rb'
require 'extensions.rb'

module EpicGames
  module Datasmith
    unless file_loaded?(__FILE__)
      # Global variables useful for debugging(running plugin without installing it):
      # $epicgames_datasmith_sketchup_plugin_main - path to plugin_main.rb to load extension from
      # $epicgames_datasmith_sketchup_dll - path to .so 
      # $epicgames_datasmith_sketchup_resources - icons
      # $epicgames_datasmith_sketchup_dev - dev tools
      # $epicgames_datasmith_sketchup_engine_path - engine path to use
      # if these variables a re not defined they are loaded as though plugin is installed

      # UNCOMMENT THIS and set path to your Engine location
      # $epicgames_datasmith_sketchup_engine_path = 'C:/ue/main/Engine'

      if $epicgames_datasmith_sketchup_dll.nil?
        extension_binary_name = if Sketchup.platform == :platform_win
          'DatasmithSketchUp.so'
        else
          'DatasmithSketchUp'
        end
        $epicgames_datasmith_sketchup_dll = File.join(File.dirname(__FILE__), 'UnrealDatasmithSketchUp', extension_binary_name)
      end

      $epicgames_datasmith_sketchup_resources = File.join(File.dirname(__FILE__), 'UnrealDatasmithSketchUp/Resources') if $epicgames_datasmith_sketchup_resources.nil?

      major, minor, build = Sketchup.version.split('.').map {|s| s.to_i}
      $epicgames_datasmith_sketchup_directlink_enabled = major >= 20 if $epicgames_datasmith_sketchup_directlink_enabled.nil?

      
      plugin_version_fpath = File.join(File.dirname(__FILE__), 'UnrealDatasmithSketchUp/version')
      plugin_version = begin 
        File.read(plugin_version_fpath).rstrip
      rescue => e
        puts "UnrealDatasmithSketchUp: Can't read version file: #{e.inspect}"
        '<unknown>'
      end

      extension_path = $epicgames_datasmith_sketchup_plugin_main || 'UnrealDatasmithSketchUp/plugin_main.rb'
      extension = SketchupExtension.new('Unreal Datasmith', extension_path)

      extension.description = "Unreal Datasmith plugin#{$epicgames_datasmith_sketchup_directlink_enabled ? ' with DirectLink':''}."
      extension.version     = plugin_version
      extension.copyright   = 'Epic Games, Inc. © 2021'
      extension.creator     = 'Epic Games'

      Sketchup.register_extension(extension, true)

      file_loaded(__FILE__)
    end
  end
end

