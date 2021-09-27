//
// Created by Sergey Chikin on 26.09.2021.
//

#include <iostream>
#include "Main.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <../lib/filesystem/CArchiveLoader.h>
#include <../lib/filesystem/CInputStream.h>
#include <../lib/CBitmapHandler.h>
#include <SDL2/SDL.h>

using namespace std;

const int BUFSIZE = 2048;

int pack(boost::filesystem::path archive, boost::filesystem::path dir) {

}

int unpack(boost::filesystem::path archive, const boost::filesystem::path& dir) {
    printf("unpacking %s\n", archive.c_str());
    auto loader = new CArchiveLoader("/", archive);

    printf("filtering\n");

    auto files = loader-> getFilteredFiles([](const ResourceID& res){ return true; });

    printf("extracting total: %lu files\n", files.size());

    ui8* buffer = new ui8[BUFSIZE];

    for(const auto& res : files) {
        auto resName = res.getName();
        auto typeString = EResTypeHelper::getEResTypeAsString(res.getType());
        printf("extracting: [%s] - %s...", typeString.c_str(), resName.c_str());

        fflush(stdout);

        auto inputStream = loader->load(res);

        auto targetFilePath = dir / resName;

        auto pair = inputStream->readAll();


        if(res.getType() == EResType::IMAGE) {
            auto surface = BitmapHandler::loadBitmapFromMem(resName.c_str(), (ui8*)pair.first.get(), (size_t)pair.second);

            SDL_SaveBMP(surface, targetFilePath.c_str());
        } else {
            boost::filesystem::ofstream ofs(targetFilePath);

            ofs.write((char*)pair.first.get(), pair.second);
        }

        printf("\t DONE - %lld bytes\n", pair.second);

        pair.first.release();
    }

    delete loader;
    delete[] buffer;

    return 0;
}

void usage() {
    cout << "usage: artool <command> <archive-path> <target-dir>\n";
    cout << "available commands:\n";
    cout << "\t - pack: packs the archive from given dir\n";
    cout << "\t - unpack: unpacks the archive to a target dir\n";
}

int main(int argc, char * argv[]) {
    if(argc < 4) {
        usage();

        return 0;
    }

    auto command = argv[1];

    auto targetPath = boost::filesystem::path(argv[3]);
    auto archivePath = boost::filesystem::path(argv[2]);

    if(strcmp(command, "pack")==0) {
        return pack(archivePath, targetPath);
    } else if(strcmp(command, "unpack")==0) {
        return unpack(archivePath, targetPath);
    } else {
        printf("unexpected command: '%s'\n\n", command);

        usage();
    }

    return 0;
}
