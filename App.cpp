#include "App.h"
#include "Maze.h"
#include "Solver.h"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <algorithm>
#include <chrono>


App::App()
{
    //empty
}

void App::run()
{
    int height;
    int width;
    int int_generationAlgorithm;
    int cellHeight;
    int cellWidth;

    std::cout << "Enter [height] [width] [GenerationAlgorithm] [cell height] [cell width]: ";
    std::cin >> height >> width >> int_generationAlgorithm >> cellHeight >> cellWidth;

    m.r = &r;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    m.make(height, width, static_cast<GenerationAlgorithm>(int_generationAlgorithm));
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

    Solver solver(&m, { 0, 0 }, { m.height - 1, m.width - 1 });
    solver.solve(SolverAlgorithm::WallFollower);
    //return; //for timing maze creation

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML works!", sf::Style::Fullscreen);
    
    sf::RenderTexture rt;
    rt.create(m.width + 1 + m.width * cellWidth, m.height + 1 + m.height * cellHeight);
    sf::VertexArray lines(sf::PrimitiveType::Lines, 2 * (m.height + m.width + 2) + 2 * m.edges);
    int n = 0;
    for (int i = 0; i <= m.height; ++i) //horizontal lines
    {
        lines[n].color = sf::Color::Red;
        lines[n].position = { 0.0f, i * (cellHeight + 1.0f) };
        lines[n + 1].color = sf::Color::Red;
        lines[n + 1].position = { m.width + 1.0f + m.width * cellWidth, i * (cellHeight + 1.0f) };
        n += 2;
    }
    for (int i = 0; i <= m.width; ++i) //vertical lines
    {
        lines[n].color = sf::Color::Red;
        lines[n].position = { i * (cellWidth + 1.0f), 0.0f };
        lines[n + 1].color = sf::Color::Red;
        lines[n + 1].position = { i * (cellWidth + 1.0f) + 1, m.height + 1.0f + m.height * cellHeight };
        n += 2;
    }

    for (int i = 0; i != m.height; ++i)
    {
        for (int j = 0; j != m.width; ++j)
        {
            const std::vector<Coord>& edges = m.graph[i][j];
            if (std::find(edges.begin(), edges.end(), Coord(i, j + 1)) != edges.end())
            {
                lines[n].color = sf::Color::Black;
                lines[n].position = { (j + 1) * (cellWidth + 1.0f), i * (cellHeight + 1.0f) + 1};
                lines[n + 1].color = sf::Color::Black;
                lines[n + 1].position = { (j + 1) * (cellWidth + 1.0f) + 1, (i + 1) * (cellHeight + 1.0f) };
                n += 2;
            }
            if (std::find(edges.begin(), edges.end(), Coord(i + 1, j)) != edges.end())
            {
                lines[n].color = sf::Color::Black;
                lines[n].position = { j * (cellWidth + 1.0f) + 1, (i + 1) * (cellHeight + 1.0f) };
                lines[n + 1].color = sf::Color::Black;
                lines[n + 1].position = { (j + 1) * (cellWidth + 1.0f), (i + 1) * (cellHeight + 1.0f) };
                n += 2;
            }
        }
    }

    rt.clear(sf::Color::Black);
    rt.draw(lines);

    //drawing solution
    Coord curr = solver.getStart();
    sf::RectangleShape rs;
    rs.setFillColor(sf::Color(128, 128, 128));
    const std::vector<Direction>& sol = solver.solution;
    for (size_t i = 0; i < sol.size(); /*empty*/)
    {
        const Coord addon = nesw_addons[sol[i++]];
        rs.setPosition(1.0f + curr.j*(cellWidth + 1) + (addon.j >> 1), 1.0f + curr.i*(cellHeight + 1) + (addon.i >> 1));
        rs.setSize({ static_cast<float>(cellWidth + (addon.j & 1)), static_cast<float>(cellHeight + (addon.i & 1)) });
        rt.draw(rs);
        curr += addon;
    }
    rs.setPosition(1.0f + curr.j * (cellWidth + 1), 1.0f + curr.i * (cellHeight + 1));
    rs.setSize({ static_cast<float>(cellWidth), static_cast<float>(cellHeight) });
    rt.draw(rs);

    //TODO TODO TODO you can make the solution draw faster (i think) by using vertexarray and quads
    //good job.

    //rendering
    rt.display();
    sf::Sprite sprite(rt.getTexture());

    window.clear(sf::Color::Black);
    window.draw(sprite);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //window.clear(sf::Color::Black);
        //window.draw(sprite);
        window.display();
    }

    return;
}