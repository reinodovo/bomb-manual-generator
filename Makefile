build: FORCE
	docker build --tag reinodovo/bomb-manual-generator . -f Dockerfile --platform linux/amd64
push: build
	docker push reinodovo/bomb-manual-generator
FORCE: ;
