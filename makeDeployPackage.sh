echo Making deploy package...
mkdir -p DeployPackage
rm -rf DeployPackage/*
cp -r src/web/* DeployPackage
cp src/cli/cli DeployPackage
echo ""{\"cli_path\": \"/src/src/cli/cli\"}"" > DeployPackage/config
tar -czf deploy.tar.gz DeployPackage/*
rm -rf DeployPackage
echo Done.