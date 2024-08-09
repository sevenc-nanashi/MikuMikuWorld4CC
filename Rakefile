task "build:msbuild" do
  puts "Running msbuild"
  sh "msbuild /p:Configuration=Release /p:Platform=x64"
end

task "build:copy" do
  require "fileutils"

  puts "Building build/"

  FileUtils.rm_rf "build"
  FileUtils.mkdir_p "build"
  FileUtils.cp_r "x64/Release", "build/MikuMikuWorld"
  FileUtils.cp "build/MikuMikuWorld/MikuMikuWorld.pdb",
               "build/__YOU_DONT_NEED_THIS__.pdb"
  FileUtils.rm "build/MikuMikuWorld/MikuMikuWorld.pdb"
end

task "build:installer" do
  puts "Building installer"
  installer = File.read("./installerBase.nsi")
  version =
    File
      .read("./MikuMikuWorld/MikuMikuWorld.rc")
      .match(/FILEVERSION (\d+),(\d+),(\d+),(\d+)/)
      .to_a[
      1..-1
    ].join(".")
  File.write("./installer.nsi", installer.gsub(/{version}/, version))

  sh "makensis installer.nsi"
end

task "build:zip" do
  puts "Building zip"
  sh "7z a -tzip MikuMikuWorld.zip MikuMikuWorld", { chdir: "build" }
end

task "build" => %w[build:msbuild build:copy build:installer build:zip]
