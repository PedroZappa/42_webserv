from setuptools import setup, find_packages

setup(
  name="Webserv",
  version="0.1.0",
  description="Python Request Test Helpers",
  author="Zedr0",
  # package_dir={"": "app"},
  # packages=find_packages(include=["app.*"]),
  # include_package_data=True,
  scripts=[
    "postings_webserv42/scripts/run.sh",
  ],
  install_requires=[
    "setuptools",
    "requests",
  ],
  # extras_require={
  #   "dev": ["debugpy", "ruff"],
  # },
)


