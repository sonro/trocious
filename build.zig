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

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const cflags = if (optimize == .Debug) debug_cflags else release_cflags;

    var src_files = SourceFiles.inDir(b.allocator, "src") catch unreachable;
    defer src_files.deinit(b.allocator);

    const lib = b.addStaticLibrary(.{
        .name = "libtrocious",
        .link_libc = true,
        .target = target,
        .optimize = optimize,
    });

    lib.installHeadersDirectory(b.path("include"), "", .{});
    lib.addIncludePath(b.path("include"));
    lib.addCSourceFiles(.{ .files = src_files.slice(), .flags = &cflags });
    b.installArtifact(lib);

    const test_exe = b.addExecutable(.{
        .name = "test",
        .link_libc = true,
        .target = target,
        .optimize = optimize,
    });

    var test_files = SourceFiles.inDir(b.allocator, "tests") catch unreachable;
    defer test_files.deinit(b.allocator);

    test_exe.addCSourceFiles(.{ .files = test_files.slice(), .flags = &debug_cflags });
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

const SourceFiles = struct {
    list: std.ArrayListUnmanaged([]u8),
    const Self = @This();

    pub fn init() SourceFiles {
        return SourceFiles{
            .list = std.ArrayListUnmanaged([]u8){},
        };
    }

    pub fn slice(self: *const Self) []const []u8 {
        return self.list.items;
    }

    pub fn deinit(self: *Self, allocator: std.mem.Allocator) void {
        for (self.list.items) |file| {
            allocator.free(file);
        }

        self.list.deinit(allocator);
    }

    pub fn inDir(allocator: std.mem.Allocator, dir_path: []const u8) !Self {
        var self = Self.init();
        errdefer self.deinit(allocator);

        var dir = try std.fs.cwd().openDir(dir_path, .{ .iterate = true });

        defer dir.close();

        var walker = try dir.walk(allocator);
        defer walker.deinit();

        while (try walker.next()) |entry| {
            // is a c file
            if (!std.mem.endsWith(u8, entry.path, ".c")) continue;
            const path = try std.fs.path.join(allocator, &.{ dir_path, entry.path });
            try self.list.append(allocator, path);
        }
        return self;
    }
};
