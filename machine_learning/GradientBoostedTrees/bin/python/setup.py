from setuptools import setup, find_packages


setup(
    name="GBDT",
    version="0.1.0",
    author="Jake Mehlman",
    description="A library for gradient boosting",
    license="MIT",
    packages=find_packages(),
    package_data={"": ["*.so"]},
    include_package_data=True,
    )
