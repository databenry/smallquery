import os
import shutil
import pathlib
import posixpath
import setuptools
from setuptools import setup, find_packages
from setuptools.command import build_ext
from glob import glob



class BazelExtension(setuptools.Extension):
    """A C/C++ extension that is defined as a Bazel BUILD target."""

    def __init__(self, name, bazel_target):
        self.bazel_target = bazel_target
        self.relpath, self.target_name = posixpath.relpath(bazel_target, "//").split(":")
        setuptools.Extension.__init__(self, name, sources=[])


class BuildBazelExtension(build_ext.build_ext):
    """A command that runs Bazel to build a C/C++ extension."""

    def run(self):
        for ext in self.extensions:
            self.bazel_build(ext)
            build_ext.build_ext.run(self)

    def bazel_build(self, ext):
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        bazel_argv = [
            'bazelisk',
            'build',
            ext.bazel_target,
            '--symlink_prefix=' + os.path.join(self.build_temp, 'bazel-'),
            '--compilation_mode=' + ('dbg' if self.debug else 'opt'),
        ]

        self.spawn(bazel_argv)

        if not ext.name.startswith("_"):
            ext.name = "_" + ext.name
        shared_lib_ext = ".so"
        shared_lib = ext.name + shared_lib_ext
        ext_bazel_bin_path = os.path.join(self.build_temp, "bazel-bin", ext.relpath, shared_lib)
        print(f'ext_bazel_bin_path = {ext_bazel_bin_path}')

        ext_dest_path = self.get_ext_fullpath(ext.name)
        ext_dest_dir = os.path.dirname(ext_dest_path)

        if not os.path.exists(ext_dest_dir):
            os.makedirs(ext_dest_dir)
        shutil.copyfile(ext_bazel_bin_path, ext_dest_path)

        package_dir = os.path.join(ext_dest_dir, 'smallquery')
        if not os.path.exists(package_dir):
            os.makedirs(package_dir)

        shutil.copyfile(
            "core/__init__.py", os.path.join(package_dir, "__init__.py")
        )

setup(
    name='smallquery',
    version='202202.15.13',
    python_requires='>=3.4',
    packages=find_packages(),
    cmdclass=dict(build_ext=BuildBazelExtension),
    ext_modules=[
        BazelExtension('smallquery', '//core:smallquery'),
    ],
)
