const std = @import("std");

const std_cflags = [_][]const u8{
    "-std=c23",
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Werror",
    "-Wno-gnu",
    "-Wno-missing-braces",
};

const debug_cflags = std_cflags ++ [_][]const u8{
    "-g",
};

const release_cflags = std_cflags ++ [_][]const u8{
    "-O3",
};

const src_files = &.{
    "src/trocious.c",
    "src/registry.c",
};

const test_files = &.{
    "tests/run.c",
    "tests/another_file.c",
};

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const cflags = if (optimize == .Debug) debug_cflags else release_cflags;

    const lib = b.addStaticLibrary(.{
        .name = "libtrocious",
        .link_libc = true,
        .target = target,
        .optimize = optimize,
    });

    lib.installHeadersDirectory(b.path("include"), "", .{});
    lib.addIncludePath(b.path("include"));
    lib.addCSourceFiles(.{ .files = src_files, .flags = &cflags });
    b.installArtifact(lib);

    const test_exe = b.addExecutable(.{
        .name = "test",
        .link_libc = true,
        .target = target,
        .optimize = optimize,
    });

    test_exe.addCSourceFiles(.{ .files = test_files, .flags = &debug_cflags });
    test_exe.linkLibrary(lib);
    b.installArtifact(test_exe);

    const run_cmd = b.addRunArtifact(test_exe);
    run_cmd.step.dependOn(b.getInstallStep());
    const run_step = b.step("test", "Run tests");
    run_step.dependOn(&run_cmd.step);

    addCleanSteps(b);
}

fn addCleanSteps(b: *std.Build) void {
    const clean_step = b.step("clean", "Remove build artifacts");
    const remove_build_artifacts = b.addRemoveDirTree(b.path("zig-out"));
    const remove_cache = b.addRemoveDirTree(b.path(".zig-cache"));
    clean_step.dependOn(&remove_build_artifacts.step);
    clean_step.dependOn(&remove_cache.step);
}
