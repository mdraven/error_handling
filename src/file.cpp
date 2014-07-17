/*
 * file.cpp
 *
 *  Created on: Jul 16, 2014
 *      Author: mdraven
 */

#include <error_handling/error_handling.hpp>

#include <cstdio>
#include <cerrno>
#include <cstring>

namespace e = error_handling;

using e::R;
using e::V;
using e::repack;
using e::if_err;

namespace other {

struct FErr {
	int err;

	FErr(int err) : err(err) {}
};

struct FEof {
	size_t sz;

	FEof(size_t sz) : sz(sz) {}
};

R<FILE*, FErr> fopen(const char* path, const char* mode) noexcept {
	FILE* handle = std::fopen(path, mode);
	if(handle == NULL)
		return FErr{errno};
	return handle;
}

R<size_t, FErr, FEof> fread(void* ptr, size_t size, size_t nmemb, FILE* stream) noexcept {
	size_t sz = std::fread(ptr, size, nmemb, stream);
	if(sz < nmemb) {
		int err = ferror(stream);
		if(err)
			return FErr{err};

		int eof = feof(stream);
		if(eof)
			return FEof{sz};
	}
	return sz;
}

R<size_t, FErr> fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream) noexcept {
	size_t sz = std::fwrite(ptr, size, nmemb, stream);
	if(sz < nmemb)
		return FErr{errno};
	return sz;
}

R<V, FErr> fclose(FILE* stream) noexcept {
	int r = std::fclose(stream);
	if(r == EOF)
		return FErr{errno};
	return V();
}

} /* namespace other */

void write() {
	namespace o = other;
	using o::FErr;

	R<FILE*, FErr> r1 = o::fopen("/tmp/error_handling", "w");

	R<V, FErr> r2 = repack<V>(std::move(r1), [](FILE* handle) {
		const char txt[] = "hello, world!";
		R<size_t, FErr> r1 = o::fwrite(txt, 1, sizeof(txt), handle);

		return repack<V>(std::move(r1), [handle](size_t i) {
			printf("Was written %lu elements\n", i);
			return o::fclose(handle);
		});
	});

	if_err<FErr>(std::move(r2), [](FErr&& err) {
		printf("Error: %s\n", std::strerror(err.err));
		return;
	});
}

void read() {
	namespace o = other;
	using o::FErr;
	using o::FEof;

	R<FILE*, FErr> r1 = o::fopen("/tmp/error_handling", "r");

	R<V, FErr> r2 = repack<V>(std::move(r1), [](FILE* handle) {
		char buf[20];
		R<size_t, FErr, FEof> r1 = o::fread(buf, 1, sizeof(buf), handle);

		R<size_t, FErr> r2 = if_err<FEof>(std::move(r1), [](FEof&& eof) {
			printf("EOF: %lu bytes were read\n", eof.sz);
			return eof.sz;
		});

		return repack<V>(std::move(r2), [buf, handle](int i) {
			printf("%.*s\n", i, buf);
			return o::fclose(handle);
		});
	});

	if_err<FErr>(std::move(r2), [](FErr&& err) {
		printf("Error: %s\n", std::strerror(err.err));
		return;
	});
}

int main() {
	write();

	read();

	return 0;
}
