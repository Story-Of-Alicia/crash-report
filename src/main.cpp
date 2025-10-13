#include <filesystem>
#include <iostream>

#include <zlib.h>

#include <boost/beast.hpp>

#include "ui_main.h"

#define REPORT_SERVER_HOST "localhost"
#define REPORT_SERVER_PORT "80"

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

typedef struct {
    std::list<std::string> files;
} report;

static Ui::main_window main_window {};
static report *to_report = nullptr;

bool upload_file(std::string const & const path, std::string const & const id)
{
    FILE *fd = fopen(path.c_str(), "r");
    if (fd == nullptr)
    {
        throw std::runtime_error(std::format("failed to open file: '{}'", path));
    }

    net::io_context ioc;
    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    auto const results = resolver.resolve(REPORT_SERVER_HOST, REPORT_SERVER_PORT);
    stream.connect(results);

    http::request<http::string_body> req {http::verb::put, REPORT_SERVER_HOST, 11};

    req.set(http::field::host, REPORT_SERVER_HOST);
    req.set(http::field::content_encoding, "deflate");
    req.set(http::field::cookie, id);

    std::string boundary = "boundaryyyyyyyyy";
    req.set(http::field::content_type, "multipart/form-data; boundary=" + boundary);

    http::write(stream, req);

    char buf[4096] = {};
    while (1)
    {
        int r = fread(buf, sizeof(*buf), sizeof(buf), fd);
        if (r == 0)
        {
            /* check if it's an error */
            if (feof(fd) != 0)
            {
                fclose(fd);
                stream.socket().shutdown(tcp::socket::shutdown_both);
                throw std::runtime_error(std::format("failed to read file: '{}'", path));
            }
            fclose(fd);
            break;
        }

        boost::asio::write(stream.socket(), boost::asio::buffer(buf, sizeof(buf)));
    }

}

void post_report(report const * const report)
{
    assert(report != nullptr);
    for (auto file : report->files)
    {

    }
}

void on_submit(bool checked)
{
    // TODO: ideally pass it as an argument
    assert(to_report != nullptr);

    main_window.statusbar->showMessage("Posting report...");
    main_window.description->setDisabled(true);
    main_window.button_submit->setDisabled(true);
    main_window.button_abort->setDisabled(true);

    try
    {
        post_report(to_report);
    } catch (const std::runtime_error & e)
    {
        main_window.statusbar->showMessage("Failed to post the report, check the log for more information.");
        fprintf(stderr, "failed to submit report:\n");
        fprintf(stderr, "%s\n", e.what());
    }

    main_window.statusbar->showMessage("Report sent!");


    main_window.button_abort->setDisabled(false);
    main_window.button_abort->setText("Close");
    /* QApplication::quit(); */
}

int main(int argc, char *argv[])
{
    /* fprintf(stdout, "Zlib Version: %s\n", zlibVersion()); */
    QApplication app(argc, argv);
    QMainWindow window;

    to_report = new report;
    QString files {};

    for (int i = 1; i < argc; ++i)
    {
        to_report->files.push_back(std::string(argv[i]));
        files.append(argv[i]);
        files.append("\n");
    }

    main_window.setupUi(&window);
    main_window.files->setText(files);

    QObject::connect(main_window.button_submit, &QPushButton::clicked, on_submit);

    window.show();
    int r = app.exec();

    delete to_report;
    return r;
}