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

task "check:translation" do
  def list_keys(file)
    file
      .split("\n")
      .filter_map do |line|
        next nil if line.start_with?("#")
        next nil unless line.include?(",")
        line.split(",")[0]
      end
  end

  files = Dir.children("./MikuMikuWorld/res/i18n")
  template = File.read("./MikuMikuWorld/res/i18n/.template.csv")
  template_keys = list_keys(template)

  coverages = {}
  files.each do |file|
    next if file == ".template.csv"

    puts "== #{file}"
    keys = list_keys(File.read("./MikuMikuWorld/res/i18n/#{file}"))
    missing_keys = template_keys - keys
    if missing_keys.size > 0
      puts "missing keys:"
      puts "  " + missing_keys.join(", ")
    end
    coverage = ((1 - missing_keys.size.to_f / template_keys.size) * 100).round(2)
    puts "Coverage: #{coverage}%"
    coverages[file.split(".")[0]] = coverage
  end
  if (output_file = ENV["GITHUB_OUTPUT"])
    puts "Writing to GITHUB_OUTPUT"
    coverages.each do |lang, coverage|
      File.write(output_file, "coverage_#{lang}=#{coverage}\n", mode: "a+")
    end
  end
end

task "check" => %w[check:translation]
